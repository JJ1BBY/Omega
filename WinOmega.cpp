#include <assert.h>
#include <malloc.h>
#include <stdlib.h>

#include <deque>
#include <map>
#include <set>
#include <string>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commctrl.h>
#include <objbase.h>
#include <shlobj.h>
#include <xinput.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

extern "C"
{
// Curses variables
#include "curses.h"

WINDOW* stdscr = NULL;
WINDOW* curscr = NULL;
int COLS = 80;
int LINES = 25;

// Omega variables
#ifndef __STDC__
#define __STDC__
#endif
#include "glob.h"

extern WINDOW *Levelw,*Dataw,*Flagw,*Timew,*Menuw,*Locw,*Morew,*Phasew;
extern WINDOW *Comwin,*Msg1w,*Msg2w,*Msg3w,*Msgw;
extern int target_on;

// Directory for Omega library files
char OMEGALIB[_MAX_PATH];
}

// Windows interface

#include "resource.h"

// Looks up a localized message string by resource ID (IDS_MSG_*). The
// resource compiler picks the STRINGTABLE block matching the current
// thread's locale (see the LANGUAGE blocks in WinOmega.rc), falling back
// to the first available block if no exact match is found. Uses a small
// rotating pool of buffers so a handful of LS() calls can be combined
// (eg. in one sprintf) without clobbering each other.
extern "C" char *LS(int id)
{
  static char buffers[4][512];
  static int next = 0;
  char *buf = buffers[next];
  next = (next + 1) % 4;

  if (LoadStringA(GetModuleHandle(NULL), id, buf, sizeof(buffers[0])) == 0)
    _snprintf(buf, sizeof(buffers[0]), "[missing string %d]", id);
  return buf;
}

// Path to saved file to load at start
char OmegaSave[_MAX_PATH] = "";

// Handles for window components
HWND wnd = 0;
HFONT font = 0;
HICON icon = 0;
HINSTANCE appInstance = 0;

// Handles for the display bitmap
HDC drawDC = 0;
HBITMAP drawBmp = 0;

// Handle for registry key for settings
HKEY settings = 0;

// The size of the display
RECT workArea = { 0,0,0,0 };
SIZE winSize = { 0,0 };

// Details of the font
LOGFONT fontSetup;
int fontSize = 11;
TEXTMETRIC fontMetrics;

// UI language override: 0 means follow the OS default, otherwise a LANGID
// explicitly picked in the setup dialog. Defaults to English so a first
// run (no "Language" registry value yet) doesn't depend on the OS locale;
// an explicit "(System default)" choice is still selectable and, once
// saved, is distinguished from "never set" by the registry value existing
// at all. originalLocale is the thread's locale before any override, used
// to restore "system default" behaviour.
DWORD uiLanguage = MAKELANGID(LANG_ENGLISH,SUBLANG_ENGLISH_UK);
LCID originalLocale = 0;

// Whether the language selection should be written to the registry so it
// persists to the next run; mirrors the "Remember this selection"
// checkbox in the setup dialog. Defaults to on so existing behaviour
// (language is always remembered) is unchanged until a user opts out.
bool saveLanguageChoice = true;

// Set of fixed width TrueType fonts
std::set<std::string> fontNames;

// Queue of input key presses
std::deque<int> inputKeys;

// Palette for text
COLORREF palette[] =
{
  RGB(0x00,0x00,0x00), // black
  RGB(0x00,0x00,0xFF), // blue
  RGB(0x00,0x66,0x11), // green
  RGB(0x00,0xCC,0xCC), // cyan
  RGB(0x88,0x00,0x00), // red 
  RGB(0xAA,0x00,0xCC), // purple
  RGB(0x99,0x55,0x00), // brown
  RGB(0xFF,0xFF,0xDD), // off white
  RGB(0xAA,0xAA,0xAA), // grey
  RGB(0x00,0xBB,0xFF), // light blue
  RGB(0x66,0xFF,0x44), // light green
  RGB(0x55,0xFF,0xFF), // light cyan
  RGB(0xFF,0x00,0x00), // light red
  RGB(0xDD,0x55,0xFF), // light purple
  RGB(0xFF,0xCC,0x00), // yellow
  RGB(0xFF,0xFF,0xFF), // white
};

// Details of the input cursor
bool cursorOn = false;
bool cursorSolid = false;
POINT cursorPos = { 0,0 };

// Last settled direction, mirrored from gamepadDirDebounce.settled below
// (see pushGamepadDirection()) -- also referenced by gamepadConfigDlgProc
// indirectly through the debounce objects.
int gamepadLastDir = 0;

// Generic "commit only once stable" debounce: some controllers report
// noisy raw values around a real physical press/release -- a d-pad/POV
// can flicker between two adjacent direction buckets right at their
// boundary, or drop to "centered" for a poll tick or two while still
// physically held; buttons can bounce the same way at the electrical
// level. Naively edge-triggering off the raw per-poll value turns one
// physical action into several queued inputs. This type instead
// requires a value to be seen for several consecutive polls before it's
// treated as the new "settled" state, and only queues an input on that
// transition.
template <typename T> struct GamepadDebounce
{
  T settled;
  T pending;
  int pendingCount;
  T zeroValue;
  GamepadDebounce(T initial, T zero) : settled(initial), pending(initial), pendingCount(0), zeroValue(zero) {}

  // Returns true (once) when raw has just become the new settled value.
  // ticksToZero/ticksAwayFromZero let the "looks released" and "looks
  // freshly pressed/changed" transitions require different amounts of
  // stability: released needs to survive a brief bounce-to-centered
  // mid-press (higher tick count), while a fresh press/new direction
  // just needs to not be pure single-tick noise (lower tick count).
  bool update(T raw, int ticksToZero, int ticksAwayFromZero)
  {
    if (raw == settled)
    {
      pending = raw;
      pendingCount = 0;
      return false;
    }
    if (raw == pending)
      pendingCount++;
    else
    {
      pending = raw;
      pendingCount = 1;
    }
    int needed = (raw == zeroValue) ? ticksToZero : ticksAwayFromZero;
    if (pendingCount >= needed)
    {
      settled = raw;
      pendingCount = 0;
      return true;
    }
    return false;
  }
};

GamepadDebounce<int> gamepadDirDebounce(0,0);
GamepadDebounce<bool> gamepadConfirmDebounce(false,false);
GamepadDebounce<bool> gamepadCancelDebounce(false,false);

// Shared debounced-direction logic for both the XInput and DirectInput
// polling paths. Releasing (settling back to 0) needs 3 consecutive
// "centered" polls (~48ms at the 16ms poll period), so a brief bounce-
// to-centered mid-press doesn't reset things; a fresh or changed
// direction needs 2 consecutive matching polls (~32ms), so noise
// straddling two adjacent direction buckets doesn't register as two
// separate presses.
void pushGamepadDirection(int dir)
{
  if (gamepadDirDebounce.update(dir,3,2) && dir != 0)
    inputKeys.push_back(dir);
  gamepadLastDir = gamepadDirDebounce.settled;
}

// DirectInput button-role mapping (confirm/cancel/run), unlike XInput's
// fixed A/B/RightShoulder there's no standard numbering across DirectInput
// devices, so these are learned per-machine via the system menu's
// "Configure Gamepad..." dialog (see showGamepadConfigDialog()) and
// persisted in the registry. -1 means "not configured" -- pollDirectInput()
// simply skips that role.
int gamepadButtonConfirm = -1;
int gamepadButtonCancel = -1;
int gamepadButtonRun = -1;

// If true, use graphics
bool graphics = true;

// Handles for the display bitmap
HDC gfxDC = 0;
HBITMAP gfxBmp = 0;

// The size of a graphics tile
SIZE gfxSize = { 32,32 };

// Mapping for graphics tiles
std::map<int,POINT> gfxMap;

// Data to initialize the graphics tile mapping
int gfxMapData[][3] =
{
  // Objects, locations and terrain
  SPACE,0,0,
  WALL,0,26,
  PORTCULLIS,33,22,
  OPEN_DOOR,32,26,
  CLOSED_DOOR,31,26,
  WHIRLWIND,42,18,
  ABYSS,7,22,
  LAVA,79,23,
  HEDGE,60,23,
  WATER,18,23, // Also RIVER
  FIRE,78,23,
  TRAP,33,10,
  LIFT,31,27,
  STAIRS_UP,42,22,
  STAIRS_DOWN,43,22,
  FLOOR,14,23, // Also ROAD
  PLAYER,0,4,
  CORPSE,45,7,
  STATUE,0,24,
  RUBBLE,55,7,
  ALTAR,34,26,
  CASH,9,7,
  PILE,12,7,
  FOOD,4,2,
  WEAPON,13,10,
  MISSILEWEAPON,60,10,
  SCROLL,75,7,
  POTION,40,8,
  ARMOR,48,0,
  SHIELD,94,0,
  CLOAK,5,0,
  BOOTS,20,0,
  STICK,28,10,
  RING,10,5,
  THING,9,6,
  ARTIFACT,42,5,
  // Terrain types
  PLAINS,9,23,
  TUNDRA,99,23,
  MOUNTAINS,117,23,
  PASS,96,21,
  CITY,84,25,
  VILLAGE,96,24,
  FOREST,54,23,
  JUNGLE,55,23,
  SWAMP,11,23,
  VOLCANO,117,22,
  CASTLE,53,26,
  TEMPLE,34,26,
  CAVES,82,22,
  DESERT,99,22,
  CHAOS_SEA,20,23,
  STARPEAK,87,23,
  DRAGONLAIR,31,12,
  MAGIC_ISLE,51,7,
  CHAIR,24,26,
  SAFE,5,7,
  FURNITURE,48,24,
  BED,25,26,
  // Monsters in effects code
  'n'|COL_RED,66,21, // nymph (and necromancer)
  's'|COL_RED,67,21, // satyr
  'I'|COL_RED,66,21, // incubus
  'S'|COL_RED,67,21, // succubus
  // Weapons in effects code
  '!'|COL_BROWN,65,1, // bolt
  '@'|COL_LIGHT_PURPLE,64,1, // ball
  '*'|COL_LIGHT_RED,68,1, // flame bolt and ball
  '^'|COL_LIGHT_BLUE,95,1, // electricity bolt and ball
  // Monsters
  'h'|COL_YELLOW,48,15, // hornet
  '@'|COL_RED,0,3, // men, priests, etc.
  'G'|COL_RED,5,4, // guardsman
  's'|COL_WHITE,75,15, // sheep
  'g'|COL_BROWN,32,19, // grunt
  't'|COL_PURPLE,56,17, // tse-tse fly
  'f'|COL_PURPLE,31,15, // fnord
  'r'|COL_BROWN,27,13, // sewer rat and rabbit
  'f'|COL_CYAN,91,14, // aggravator fungus
  'b'|COL_BRIGHT_WHITE|COL_FG_BLINK,78,14, // blipper
  'g'|COL_GREEN,88,3, // goblin
  'p'|COL_RED,54,15, // phantasticon
  'R'|COL_GREY,2,18, // household robot
  'g'|COL_WHITE,8,11, // pencil-necked geek
  'b'|COL_GREEN,8,19, // borogrove
  'q'|COL_BROWN,12,17, // quail
  'b'|COL_BROWN,25,13, // badger and bear
  'H'|COL_BROWN,37,19, // hawk
  'h'|COL_BROWN,27,19, // hyena and horse
  'd'|COL_BROWN,27,19, // deer and doberman death-hound
  'c'|COL_BROWN,27,19, // camel and radeligian cateagle
  'a'|COL_BROWN,42,16, // anteater
  't'|COL_CYAN,33,17, // trout
  'b'|COL_CYAN,34,17, // striped bass
  'p'|COL_PURPLE,15,17, // parrot
  'n'|COL_GREY,23,4, // ninjas, night gaunt and transparent nasty
  'e'|COL_GREEN,10,16, // floating eye and etheric grue
  't'|COL_GREEN,16,19, // tove
  'g'|COL_GREY,20,20, // ghost
  'e'|COL_RED,18,3, // enchanter
  'f'|COL_GREY,90,14, // murk fungus
  'G'|COL_GREEN,84,3, // goblin chieftain and goblin shaman
  'w'|COL_BROWN,76,15, // wolf
  'a'|COL_RED,21,19, // giant soldier ant
  'e'|COL_GREY,61,17, // elephant
  's'|COL_LIGHT_RED,13,19, // salamander
  'C'|COL_GREEN,30,17, // catoblepas
  'f'|COL_LIGHT_BLUE,52,13, // lesser frost demon
  'a'|COL_YELLOW,23,15, // acid cloud
  'p'|COL_GREY,21,20, // phantom
  'K'|COL_LIGHT_GREEN,82,3, // The Goblin King
  'P'|COL_PURPLE,59,14, // pterodactyl
  'l'|COL_YELLOW,77,15, // lion
  'b'|COL_RED,67,21, // brigand
  'm'|COL_RED,27,16, // mamba
  'M'|COL_BLUE,6,15, // man o' war
  'T'|COL_GREY,21,15, // thought-form
  'M'|COL_YELLOW,64,16, // manticore
  'T'|COL_BROWN,45,11, // tasmanian devil
  'a'|COL_GREY,4,18, // automatum minor
  's'|COL_GREEN,0,15, // denebian slime devil
  'j'|COL_PURPLE,36,19, // jub-jub bird
  'h'|COL_GREY,22,20, // haunt
  'c'|COL_GREEN,16,19, // giant crocodile
  't'|COL_BROWN,37,15, // torpor beast
  'F'|COL_GREY,25,15, // astral fuzzy
  'l'|COL_BLUE,30,4, // servant of law
  'c'|COL_RED,34,4, // servant of chaos
  's'|COL_YELLOW,48,15, // swarm
  'b'|COL_GREY,39,15, // ban sidhe
  'd'|COL_LIGHT_RED,13,12, // dragonette and outer circle demon
  't'|COL_LIGHT_BLUE,8,12, // tesla monster
  'W'|COL_LIGHT_RED,13,12, // wyvern and lava worm
  'f'|COL_WHITE,30,14, // frost demon
  'S'|COL_GREY,21,20, // spectre and shadow spirit
  'B'|COL_GREEN,40,21, // bog thing
  'V'|COL_GREY,70,21, // astral vampire
  'm'|COL_PURPLE,65,16, // manaburst
  'm'|COL_GREY,55,17, // mirrorshade
  'F'|COL_WHITE|COL_BG_RED,20,14, // fire elemental
  'A'|COL_WHITE|COL_BG_BLUE,21,14, // air elemental
  'W'|COL_WHITE|COL_BG_BLUE,24,14, // water elemental
  'E'|COL_WHITE|COL_BG_BROWN,23,14, // earth elemental
  'b'|COL_YELLOW|COL_BG_BROWN,71,18, // bandersnatch
  'L'|COL_BLACK|COL_BG_WHITE,35,16, // lich
  'T'|COL_LIGHT_GREEN|COL_BG_BLUE,8,19, // triton
  'T'|COL_GREEN|COL_BG_BROWN,47,16, // triceratops
  'R'|COL_YELLOW|COL_BG_WHITE,9,21, // rakshasa
  'S'|COL_GREEN|COL_BG_RED,27,16, // demon serpent
  'a'|COL_YELLOW|COL_BG_WHITE,65,21, // angel
  'n'|COL_BLACK|COL_BG_WHITE,23,4, // ninja (chunin) and ninja (jonin)
  'B'|COL_BLACK|COL_BG_BROWN,62,17, // behemoth
  'N'|COL_BLACK|COL_BG_WHITE,6,20, // nazgul
  'U'|COL_BLACK|COL_BG_WHITE,68,16, // unicorn
  'r'|COL_BLACK|COL_BG_BROWN,49,16, // rodent of unusual size
  'F'|COL_BLACK|COL_BG_WHITE,26,19, // illusory fiend
  'W'|COL_GREY|COL_BG_RED,47,17, // Great Wyrm
  'F'|COL_GREY|COL_BG_RED,36,21, // flame devil
  'S'|COL_YELLOW|COL_BG_BROWN,28,19, // sandman
  'm'|COL_RED|COL_BG_WHITE,54,17, // mirror master
  'G'|COL_GREY|COL_BG_GREEN,14,16, // elder etheric grue
  'S'|COL_GREY|COL_BG_GREEN,26,16, // loathly serpent
  'Z'|COL_BLACK|COL_BG_WHITE,26,20, // zombie overlord
  'r'|COL_GREY|COL_BG_BROWN,44,18, // ricochet
  'D'|COL_GREY|COL_BG_RED,21,12, // inner circle demon and dragon
  'f'|COL_YELLOW|COL_BG_WHITE,19,14, // fairy
  'A'|COL_GREY|COL_BG_WHITE,8,18, // automatum major
  'J'|COL_BROWN|COL_BG_RED,41,16, // jabberwock
  'F'|COL_LIGHT_BLUE|COL_BG_WHITE,37,21, // frost demon lord
  'T'|COL_YELLOW|COL_BG_BLUE,73,18, // tigershark
  'S'|COL_BLACK|COL_BG_WHITE,34,15, // shadow slayer
  'C'|COL_GREY|COL_BG_BROWN,9,19, // coma beast
  'A'|COL_YELLOW|COL_BG_WHITE,62,21, // high angel and archangel
  'J'|COL_GREY|COL_BG_BROWN,23,13, // jotun
  'i'|COL_BLACK|COL_BG_WHITE,72,21, // invisible slayer
  'W'|COL_GREEN|COL_BG_RED,29,12, // king wyvern
  'd'|COL_BLACK|COL_BG_WHITE,85,1, // deathstar
  'T'|COL_YELLOW|COL_BG_WHITE,6,4, // high thaumaturgist
  'V'|COL_BLACK|COL_BG_RED,18,20, // vampire lord
  'D'|COL_BLACK|COL_BG_RED,31,13, // demon prince
  'D'|COL_BLACK|COL_BG_WHITE,58,20, // Death
  'E'|COL_BLACK|COL_BG_WHITE,65,14, // The Eater of Magic
  'L'|COL_BRIGHT_WHITE|COL_BG_BLUE,18,3, // The LawBringer
  'D'|COL_BRIGHT_WHITE|COL_BG_RED,47,12, // The Dragon Lord
  'D'|COL_WHITE|COL_BG_RED,7,21, // The Demon Emperor
  'E'|COL_BROWN|COL_BG_WHITE,4,3, // The Elemental Lord of Earth
  'A'|COL_LIGHT_BLUE|COL_BG_WHITE,20,3, // The Elemental Lord of Air
  'W'|COL_BLUE|COL_BG_WHITE,9,3, // The Elemental Lord of Water
  'F'|COL_LIGHT_RED|COL_BG_WHITE,15,4, // The Elemental Lord of Fire
  'M'|COL_PURPLE|COL_BG_WHITE,89,21, // The Elemental Master
};

BYTE* offsetDWord(BYTE* ptr)
{
  int offset = (int)(DWORD_PTR)ptr;
  if (offset & 3)
    return ptr + (4 - (offset & 3));
  return ptr;
}

// Show a dialog with the resource changed to an appropriate font name and size.
// Note that different possibilities in variations on the dialog format are not checked!
int showDialog(HINSTANCE instance, int resId, HWND parent, DLGPROC proc)
{
  int code = -1;

  NONCLIENTMETRICS ncm;
  ::ZeroMemory(&ncm,sizeof ncm);
  ncm.cbSize = sizeof ncm;
  ::SystemParametersInfo(SPI_GETNONCLIENTMETRICS,sizeof ncm,&ncm,0);
  WCHAR fontName[256];
  MultiByteToWideChar(CP_ACP,0,
    ncm.lfMessageFont.lfFaceName,strlen(ncm.lfMessageFont.lfFaceName)+1,fontName,256);

  HRSRC resInfo = FindResource(instance,MAKEINTRESOURCE(resId),RT_DIALOG);
  if (resInfo != 0)
  {
    DWORD resSize = SizeofResource(instance,resInfo);
    HGLOBAL resGlobal = LoadResource(instance,resInfo);
    if (resGlobal != 0)
    {
      BYTE* resMem = (BYTE*)LockResource(resGlobal);
      if (resMem != NULL)
      {
        HGLOBAL copyGlobal = GlobalAlloc(GMEM_ZEROINIT,resSize+64);
        if (copyGlobal != 0)
        {
          BYTE* copyMem = (BYTE*)GlobalLock(copyGlobal);
          if (copyMem != 0)
          {
            int titleLen = (wcslen(((WCHAR*)resMem)+15)+1)*sizeof(WCHAR);
            int copy1Size = (18*sizeof(WORD))+titleLen;
            int font1Len = (wcslen((WCHAR*)(resMem+copy1Size))+1)*sizeof(WCHAR);
            int copy2Size = resSize-copy1Size-font1Len;
            int font2Len = (wcslen(fontName)+1)*sizeof(WCHAR);

            memcpy(copyMem,resMem,copy1Size);
            wcscpy((WCHAR*)(copyMem+copy1Size),fontName);
            memcpy(offsetDWord(copyMem+copy1Size+font2Len),offsetDWord(resMem+copy1Size+font1Len),copy2Size);
            *((WORD*)(copyMem+copy1Size-(3*sizeof(WORD)))) = 9;
            GlobalUnlock(copyGlobal);
          }
          code = DialogBoxIndirect(instance,(LPDLGTEMPLATE)copyGlobal,parent,proc);
          GlobalFree(copyGlobal);
        }
      }
    }
  }

  return code;
}

void fatal(char* msg)
{
  MessageBox(0,msg,"Omega",MB_ICONERROR|MB_OK);
  exit(1);
}

void drawCursor(void)
{
  SetTextColor(drawDC,RGB(0,0,0));
  SetBkColor(drawDC,palette[cursorSolid ? 7 : 0]);
  TextOut(drawDC,cursorPos.x,cursorPos.y," ",1);
  InvalidateRect(wnd,NULL,FALSE);
}

void drawRect(int x, int y, int w, int h, COLORREF colour)
{
  SetBkColor(drawDC,colour);
  RECT r = { x,y,x+w,y+h };
  ExtTextOut(drawDC,0,0,ETO_OPAQUE,&r,"",0,NULL);
}

void copyLibFile(const char* name)
{
  // Copy the file if it does not exist
  char fromPath[_MAX_PATH];
  sprintf(fromPath,"%s\\%s",OMEGALIB,name);
  CopyFile(fromPath,name,TRUE);

  // Remove any read-only etc. flags
  SetFileAttributes(name,FILE_ATTRIBUTE_NORMAL);
}

// XInputGetState is resolved dynamically instead of via static import.
// Only xinput9_1_0.lib ships as an import library in this SDK, but its
// compatibility shim fails to enumerate some Bluetooth/HID XInput-
// compatible controllers that the full XInput 1.4 driver stack sees
// fine -- confirmed via gamepad_debug.log: calling XInputGetState
// through the static 9.1.0 import returned ERROR_DEVICE_NOT_CONNECTED
// on all four user slots for a controller Windows' own "Game
// Controllers" panel showed as connected and OK. xinput1_4.dll (ships
// with Windows 8+) is tried first and falls back to xinput9_1_0.dll,
// which covers both cases without needing an xinput1_4.lib that isn't
// available to link against.
typedef DWORD (WINAPI *PFN_XInputGetState)(DWORD,XINPUT_STATE*);
PFN_XInputGetState pXInputGetState = NULL;

#ifdef OMEGA_GAMEPAD_DEBUG
// Temporary diagnostic logging for gamepad detection issues. Writes to
// gamepad_debug.log in the current directory, which by the time this
// can run is always the %APPDATA%\Omega directory (see
// SetCurrentDirectory() in WinMain), same place omega.log/omega.hi
// live. Logs a heartbeat every ~1s (20 polls at the 50ms timer period)
// plus immediately on any connect/disconnect transition or state
// change, so it's easy to tell "timer never fires" apart from
// "XInputGetState/DirectInput never succeeds" apart from "succeeds but
// buttons look wrong".
static void gamepadDebugLog(const char* fmt, ...)
{
  FILE* f = fopen("gamepad_debug.log","a");
  if (f == NULL)
    return;
  va_list args;
  va_start(args,fmt);
  vfprintf(f,fmt,args);
  va_end(args);
  fprintf(f,"\n");
  fclose(f);
}
#endif

void initXInput()
{
  const char* dllNames[] = { "xinput1_4.dll","xinput9_1_0.dll","xinput1_3.dll" };
  for (int i = 0; i < 3 && pXInputGetState == NULL; i++)
  {
    HMODULE dll = LoadLibraryA(dllNames[i]);
    if (dll != NULL)
    {
      pXInputGetState = (PFN_XInputGetState)GetProcAddress(dll,"XInputGetState");
#ifdef OMEGA_GAMEPAD_DEBUG
      if (pXInputGetState != NULL)
      {
        FILE* f = fopen("gamepad_debug.log","a");
        if (f != NULL)
        {
          fprintf(f,"initXInput: resolved XInputGetState from %s\n",dllNames[i]);
          fclose(f);
        }
      }
#endif
    }
  }
}

// DirectInput fallback for controllers XInput can't see at all (confirmed
// via gamepad_debug.log: a Bluetooth LE "XInput compatible" pad that
// Windows' own Game Controllers panel lists as connected/OK still gets
// ERROR_DEVICE_NOT_CONNECTED from XInputGetState on every user slot,
// through both xinput1_4.dll and xinput9_1_0.dll). The Game Controllers
// panel is itself a DirectInput device list, so if a controller shows up
// there, DirectInput can see it even when XInput can't.
LPDIRECTINPUT8 directInput = NULL;
LPDIRECTINPUTDEVICE8 diJoystick = NULL;

BOOL CALLBACK enumJoystickCallback(const DIDEVICEINSTANCE* inst, VOID* context)
{
  if (FAILED(directInput->CreateDevice(inst->guidInstance,&diJoystick,NULL)))
    return DIENUM_CONTINUE;
  return DIENUM_STOP;
}

BOOL CALLBACK enumAxesCallback(const DIDEVICEOBJECTINSTANCE* inst, VOID* context)
{
  DIPROPRANGE range;
  ZeroMemory(&range,sizeof range);
  range.diph.dwSize = sizeof range;
  range.diph.dwHeaderSize = sizeof range.diph;
  range.diph.dwHow = DIPH_BYID;
  range.diph.dwObj = inst->dwType;
  range.lMin = -1000;
  range.lMax = 1000;
  diJoystick->SetProperty(DIPROP_RANGE,&range.diph);
  return DIENUM_CONTINUE;
}

void initDirectInput(HINSTANCE instance, HWND wnd)
{
  if (FAILED(DirectInput8Create(instance,DIRECTINPUT_VERSION,IID_IDirectInput8,(VOID**)&directInput,NULL)))
    return;
  directInput->EnumDevices(DI8DEVCLASS_GAMECTRL,enumJoystickCallback,NULL,DIEDFL_ATTACHEDONLY);
  if (diJoystick == NULL)
    return;
  diJoystick->SetDataFormat(&c_dfDIJoystick2);
  diJoystick->SetCooperativeLevel(wnd,DISCL_NONEXCLUSIVE|DISCL_BACKGROUND);
  diJoystick->EnumObjects(enumAxesCallback,NULL,DIDFT_AXIS);
  diJoystick->Acquire();
#ifdef OMEGA_GAMEPAD_DEBUG
  FILE* f = fopen("gamepad_debug.log","a");
  if (f != NULL)
  {
    fprintf(f,"initDirectInput: device found and acquired\n");
    fclose(f);
  }
#endif
}

// A "control" learned during calibration is either a digital button
// (0-127) or an analog axis being pushed past a threshold, encoded as
// 1000+axisIndex (0=Z, 1=slider0, 2=slider1) so triggers that a
// controller reports as analog (eg. Joy-Con-style ZR) rather than as a
// digital button still work. -1 means unconfigured/none.
bool gamepadControlHeld(const DIJOYSTATE2& js, int control)
{
  // rgbButtons only has 128 elements -- controls 128-999 are not a
  // valid button index (they used to fall through into
  // rgbButtons[control] here, reading past the end of the array and
  // occasionally "detecting" a phantom press from whatever garbage
  // byte happened to have its high bit set).
  if (control >= 0 && control < 128)
    return (js.rgbButtons[control] & 0x80) != 0;
  if (control >= 1000 && control <= 1002)
  {
    switch (control - 1000)
    {
    case 0: return js.lZ > 500;
    case 1: return js.rglSlider[0] > 500;
    case 2: return js.rglSlider[1] > 500;
    }
  }
  return false;
}

// Formats a control id (see gamepadControlHeld()) as a short display
// string for the gamepad config dialog's status labels.
void formatGamepadControl(int control, char* buf, size_t bufSize)
{
  if (control < 0)
    strcpy(buf,LS(IDS_UI_GAMEPAD_NONE));
  else if (control < 1000)
    _snprintf(buf,bufSize,"%s %d",LS(IDS_UI_GAMEPAD_BUTTON),control+1);
  else
    _snprintf(buf,bufSize,"%s %d",LS(IDS_UI_GAMEPAD_AXIS),control-1000+1);
}

// Which role (if any) the gamepad config dialog is currently waiting on
// a button press for. Checked/advanced from that dialog's WM_TIMER
// handler, so the modal dialog's own message pump keeps running instead
// of blocking in a wait loop.
enum GamepadAssignTarget { GA_NONE, GA_CONFIRM, GA_CANCEL, GA_RUN };
GamepadAssignTarget gamepadAssignTarget = GA_NONE;
bool gamepadAssignWasHeld[1003];

void gamepadAssignBegin(GamepadAssignTarget target)
{
  gamepadAssignTarget = target;
  DIJOYSTATE2 js;
  ZeroMemory(&js,sizeof js);
  diJoystick->Poll();
  if (SUCCEEDED(diJoystick->GetDeviceState(sizeof js,&js)))
  {
    for (int c = 0; c < 1003; c++)
      gamepadAssignWasHeld[c] = gamepadControlHeld(js,c);
  }
}

// Called every gamepad config dialog timer tick while gamepadAssignTarget
// is set. Returns the newly-pressed control id, or -1 if nothing new was
// pressed this tick.
int gamepadAssignPoll(const DIJOYSTATE2& js)
{
  for (int c = 0; c < 1003; c++)
  {
    bool held = gamepadControlHeld(js,c);
    if (held && !gamepadAssignWasHeld[c])
      return c;
    gamepadAssignWasHeld[c] = held;
  }
  return -1;
}

void gamepadSaveConfig()
{
  RegSetValueEx(settings,"Gamepad Confirm",0,REG_DWORD,(BYTE*)&gamepadButtonConfirm,sizeof gamepadButtonConfirm);
  RegSetValueEx(settings,"Gamepad Cancel",0,REG_DWORD,(BYTE*)&gamepadButtonCancel,sizeof gamepadButtonCancel);
  RegSetValueEx(settings,"Gamepad Run",0,REG_DWORD,(BYTE*)&gamepadButtonRun,sizeof gamepadButtonRun);
}

// Polling helper for the DirectInput fallback device. While
// OMEGA_GAMEPAD_DEBUG is on this dumps every button/POV/axis raw value
// on change, so the (device-specific, unlike XInput) button numbering
// can be read off directly instead of guessed at.
void pollDirectInput()
{
  if (diJoystick == NULL)
    return;

  diJoystick->Poll();
  DIJOYSTATE2 js;
  HRESULT hr = diJoystick->GetDeviceState(sizeof js,&js);
  if (FAILED(hr))
  {
    diJoystick->Acquire();
    return;
  }

#ifdef OMEGA_GAMEPAD_DEBUG
  {
    static DIJOYSTATE2 last;
    static bool haveLast = false;
    if (!haveLast || memcmp(&last,&js,sizeof js) != 0)
    {
      char buttons[129];
      for (int i = 0; i < 128; i++)
        buttons[i] = (js.rgbButtons[i] & 0x80) ? '1' : '0';
      buttons[128] = '\0';
      gamepadDebugLog("DI lX=%ld lY=%ld POV0=%lu buttons=%s",
        js.lX,js.lY,js.rgdwPOV[0],buttons);
      last = js;
      haveLast = true;
    }
  }
#endif

  int dx = 0, dy = 0;
  if (js.rgdwPOV[0] != (DWORD)-1)
  {
    // POV is in hundredths of a degree, 0 = up, clockwise
    DWORD pov = js.rgdwPOV[0];
    if (pov > 31500 || pov < 4500) dy -= 1;
    if (pov > 4500 && pov < 13500) dx += 1;
    if (pov > 13500 && pov < 22500) dy += 1;
    if (pov > 22500 && pov < 31500) dx -= 1;
  }
  else
  {
    if (js.lX > 300) dx = 1;
    else if (js.lX < -300) dx = -1;
    if (js.lY > 300) dy = 1;
    else if (js.lY < -300) dy = -1;
  }

  static const int dirDigit[3][3] =
  {
    { '7','8','9' },
    { '4', 0 ,'6' },
    { '1','2','3' },
  };
  static const int dirRun[3][3] =
  {
    { 'Y','K','U' },
    { 'H', 0 ,'L' },
    { 'B','J','N' },
  };
  // Unlike XInput, DirectInput has no standard button numbering, so
  // these roles come from the system menu's "Configure Gamepad..."
  // dialog (persisted in the registry) rather than being hardcoded.
  // -1 (unconfigured) just means that role never fires.
  bool runHeld = gamepadControlHeld(js,gamepadButtonRun);
  int dir = (runHeld ? dirRun : dirDigit)[dy+1][dx+1];
  pushGamepadDirection(dir);

  bool aHeld = gamepadControlHeld(js,gamepadButtonConfirm);
  bool bHeld = gamepadControlHeld(js,gamepadButtonCancel);
  if (gamepadConfirmDebounce.update(aHeld,3,2) && aHeld)
  {
    // 'y' answers yes/no decisions (ynq() et al only accept y/n/q/ESC);
    // RETURN dismisses the far more common "-More-" pager prompts
    // (morewait() only accepts space/RETURN, not 'y'). Whichever one
    // the current context doesn't want is simply ignored by its input
    // loop and left queued for the next prompt -- in the plain dungeon/
    // city command loop a stray RETURN is an explicit documented no-op
    // (command1.c: case 10/13: SKIP_MONSTERS), so there's no harmful
    // side effect either way.
    inputKeys.push_back('y');
    inputKeys.push_back('\n');
  }
  if (gamepadCancelDebounce.update(bHeld,3,2) && bHeld)
  {
    // ESCAPE alone, not 'n': ynq()-style decisions already treat ESCAPE
    // as equivalent to declining/quitting (see scr.c's ynq() -- it
    // folds ESCAPE into the same case as 'q'), and most menus (lettered
    // option lists, shop screens, etc.) only accept ESCAPE to back out,
    // not 'n'. Sending both here would occasionally leak 'n' as a
    // southeast move when nothing is actually waiting on input (n is
    // also a movement key on the main dungeon/city command table).
    inputKeys.push_back(27);
  }
}

// Polls gamepad 0 (via XInput) for movement and yes/no confirmation.
// Directions are mapped to the same numeric-keypad digits ('1'-'9') that
// the keyboard's numpad and vi-key equivalents already produce (see
// help4.txt/help12.txt), so no changes are needed anywhere the game
// itself reads moves. A and B map to 'y'/'n', matching the ynq()
// prompts used throughout the game.
//
// Holding the right shoulder button while pressing a direction sends the
// uppercase vi-key ('H','J','K','L','B','N','Y','U' -- see command1.c's
// p_process(), which sets FAST_MOVE and lets the main loop keep
// re-issuing that same move each turn until an obstacle/RUNSTOP
// condition clears it) instead of the plain digit, so a single
// button-hold + direction runs until something interesting happens,
// same as Shift+direction on a keyboard. Mirrors the "hold R for
// dash/precise diagonal movement" convention used by the Mystery
// Dungeon (Shiren/Torneko) series -- A/B are left alone as OK/Cancel
// (y/n here), matching that series' own A=decide/B=cancel mapping.
bool gamepadConfigDialogOpen = false;

void pollGamepad()
{
  // The gamepad config dialog polls diJoystick directly for its own
  // live test-area display and button-assignment detection; suppress
  // ordinary gameplay input injection while it's open so pressing
  // buttons to calibrate doesn't also move the player around.
  if (gamepadConfigDialogOpen)
    return;

  if (pXInputGetState == NULL)
    return;

  // Scan all four XInput user indices rather than assuming slot 0 --
  // a controller (especially Bluetooth) isn't guaranteed to land on 0,
  // particularly if other controllers were connected previously.
  XINPUT_STATE state;
  bool connected = false;
  DWORD connectedIndex = 0;
  DWORD errors[XUSER_MAX_COUNT] = { 0,0,0,0 };
  for (DWORD i = 0; i < XUSER_MAX_COUNT && !connected; i++)
  {
    ZeroMemory(&state,sizeof state);
    DWORD result = pXInputGetState(i,&state);
    errors[i] = result;
    connected = (result == ERROR_SUCCESS);
    if (connected)
      connectedIndex = i;
  }

#ifdef OMEGA_GAMEPAD_DEBUG
  {
    static bool everConnected = false;
    static int pollCount = 0;
    static bool lastConnected = false;
    pollCount++;
    if (connected != lastConnected || (pollCount % 20) == 0)
    {
      gamepadDebugLog("poll #%d connected=%d slot=%lu errors=[%lu,%lu,%lu,%lu]",
        pollCount,connected,connectedIndex,errors[0],errors[1],errors[2],errors[3]);
      if (connected)
      {
        everConnected = true;
        gamepadDebugLog("  buttons=0x%04X LX=%d LY=%d",
          state.Gamepad.wButtons,state.Gamepad.sThumbLX,state.Gamepad.sThumbLY);
      }
    }
    lastConnected = connected;
    (void)everConnected;
  }
#endif

  if (!connected)
  {
    pollDirectInput();
    return;
  }

  WORD buttons = state.Gamepad.wButtons;
  int dx = 0, dy = 0;
  if (buttons & XINPUT_GAMEPAD_DPAD_LEFT) dx -= 1;
  if (buttons & XINPUT_GAMEPAD_DPAD_RIGHT) dx += 1;
  if (buttons & XINPUT_GAMEPAD_DPAD_UP) dy -= 1;
  if (buttons & XINPUT_GAMEPAD_DPAD_DOWN) dy += 1;
  if (dx == 0 && dy == 0)
  {
    // Fall back to the left thumbstick if the d-pad isn't in use
    SHORT sx = state.Gamepad.sThumbLX;
    SHORT sy = state.Gamepad.sThumbLY;
    if (sx > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) dx = 1;
    else if (sx < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) dx = -1;
    if (sy > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) dy = -1;
    else if (sy < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) dy = 1;
  }

  // Row 0 = up (dy=-1), row 2 = down (dy=+1); column 0 = left (dx=-1),
  // column 2 = right (dx=+1) -- matches command1.c's '7'/'8'/'9' (up
  // row) and '1'/'2'/'3' (down row) digit-key layout.
  static const int dirDigit[3][3] =
  {
    { '7','8','9' },
    { '4', 0 ,'6' },
    { '1','2','3' },
  };
  static const int dirRun[3][3] =
  {
    { 'Y','K','U' },
    { 'H', 0 ,'L' },
    { 'B','J','N' },
  };
  bool runHeld = (buttons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0;
  int dir = (runHeld ? dirRun : dirDigit)[dy+1][dx+1];
#ifdef OMEGA_GAMEPAD_DEBUG
  bool willPush = (dir != 0 && dir != gamepadLastDir);
#endif
  pushGamepadDirection(dir);
#ifdef OMEGA_GAMEPAD_DEBUG
  if (willPush)
    gamepadDebugLog("  -> pushed direction '%c' (run=%d, dx=%d dy=%d)",dir,runHeld,dx,dy);
#endif

  bool aHeld = (buttons & XINPUT_GAMEPAD_A) != 0;
  bool bHeld = (buttons & XINPUT_GAMEPAD_B) != 0;
  if (gamepadConfirmDebounce.update(aHeld,3,2) && aHeld)
  {
    // See the DirectInput path's comment on why both 'y' and RETURN are
    // queued: ynq()-style decisions only accept y/n/q/ESC, while the far
    // more common morewait() "-More-" pager only accepts space/RETURN.
    inputKeys.push_back('y');
    inputKeys.push_back('\n');
#ifdef OMEGA_GAMEPAD_DEBUG
    gamepadDebugLog("  -> pushed 'y'+RETURN (A pressed)");
#endif
  }
  if (gamepadCancelDebounce.update(bHeld,3,2) && bHeld)
  {
    // See the DirectInput path's comment: ESCAPE alone, not 'n' -- ynq()
    // already treats ESCAPE as declining, and 'n' risks leaking through
    // as a southeast move when nothing is actually waiting on input.
    inputKeys.push_back(27);
#ifdef OMEGA_GAMEPAD_DEBUG
    gamepadDebugLog("  -> pushed ESCAPE (B pressed)");
#endif
  }
}

// Refreshes the three role status labels from the current
// gamepadButtonConfirm/Cancel/Run values.
void updateGamepadStatusLabels(HWND hwnd)
{
  char buf[64];
  formatGamepadControl(gamepadButtonConfirm,buf,sizeof buf);
  SetWindowText(GetDlgItem(hwnd,IDC_GAMEPAD_CONFIRM_STATUS),buf);
  formatGamepadControl(gamepadButtonCancel,buf,sizeof buf);
  SetWindowText(GetDlgItem(hwnd,IDC_GAMEPAD_CANCEL_STATUS),buf);
  formatGamepadControl(gamepadButtonRun,buf,sizeof buf);
  SetWindowText(GetDlgItem(hwnd,IDC_GAMEPAD_RUN_STATUS),buf);
}

// Live test-area drawing for the gamepad config dialog, loosely modeled
// on Windows' own Game Controllers properties/test page: an X/Y
// crosshair box, a POV compass, and a grid of numbered button circles
// that light up red while held.
void drawGamepadTestArea(HDC dc, const RECT* rect)
{
  FillRect(dc,rect,(HBRUSH)GetStockObject(WHITE_BRUSH));
  SetBkMode(dc,TRANSPARENT);

  DIJOYSTATE2 js;
  ZeroMemory(&js,sizeof js);
  bool haveState = false;
  if (diJoystick != NULL)
  {
    diJoystick->Poll();
    haveState = SUCCEEDED(diJoystick->GetDeviceState(sizeof js,&js));
  }

  if (!haveState)
  {
    RECT r = *rect;
    DrawTextA(dc,LS(IDS_UI_GAMEPAD_NO_CONTROLLER),-1,&r,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
    return;
  }

  // X/Y axis crosshair box
  int boxLeft = rect->left+14, boxTop = rect->top+10, boxSize = 80;
  Rectangle(dc,boxLeft,boxTop,boxLeft+boxSize,boxTop+boxSize);
  int cx = boxLeft+boxSize/2 + (js.lX*(boxSize/2))/1000;
  int cy = boxTop+boxSize/2 + (js.lY*(boxSize/2))/1000;
  MoveToEx(dc,cx-6,cy,NULL); LineTo(dc,cx+7,cy);
  MoveToEx(dc,cx,cy-6,NULL); LineTo(dc,cx,cy+7);

  // POV compass
  int povCx = boxLeft+boxSize+70, povCy = boxTop+boxSize/2, povR = 36;
  Ellipse(dc,povCx-povR,povCy-povR,povCx+povR,povCy+povR);
  int povDx = 0, povDy = 0;
  if (js.rgdwPOV[0] != (DWORD)-1)
  {
    DWORD pov = js.rgdwPOV[0];
    if (pov > 31500 || pov < 4500) povDy = -1;
    if (pov > 4500 && pov < 13500) povDx = 1;
    if (pov > 13500 && pov < 22500) povDy = 1;
    if (pov > 22500 && pov < 31500) povDx = -1;
  }
  int dotX = povCx + povDx*(povR*3/4);
  int dotY = povCy + povDy*(povR*3/4);
  HBRUSH povBrush = CreateSolidBrush(RGB(220,40,40));
  HBRUSH oldBrush = (HBRUSH)SelectObject(dc,povBrush);
  Ellipse(dc,dotX-4,dotY-4,dotX+4,dotY+4);
  SelectObject(dc,oldBrush);
  DeleteObject(povBrush);

  // Numbered button grid
  int btnTop = boxTop+boxSize+22;
  int btnR = 9;
  int perRow = 8;
  for (int i = 0; i < 32; i++)
  {
    int bx = rect->left+22+(i%perRow)*30;
    int by = btnTop+(i/perRow)*24;
    if (by+btnR > rect->bottom-4)
      break;
    bool held = (js.rgbButtons[i] & 0x80) != 0;
    HBRUSH br = CreateSolidBrush(held ? RGB(220,40,40) : RGB(225,225,225));
    HBRUSH old = (HBRUSH)SelectObject(dc,br);
    Ellipse(dc,bx-btnR,by-btnR,bx+btnR,by+btnR);
    SelectObject(dc,old);
    DeleteObject(br);
    char num[4];
    _snprintf(num,sizeof num,"%d",i+1);
    RECT tr = { bx-btnR,by-btnR,bx+btnR,by+btnR };
    SetTextColor(dc,held ? RGB(255,255,255) : RGB(0,0,0));
    DrawTextA(dc,num,-1,&tr,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
    SetTextColor(dc,RGB(0,0,0));
  }
}

// Dialog procedure for IDD_GAMEPAD -- see showGamepadConfigDialog().
INT_PTR CALLBACK gamepadConfigDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg)
  {
  case WM_INITDIALOG:
    updateGamepadStatusLabels(hwnd);
    SetTimer(hwnd,1,50,NULL);
    return TRUE;

  case WM_DRAWITEM:
    if (wParam == IDC_GAMEPAD_TESTAREA)
    {
      DRAWITEMSTRUCT* dis = (DRAWITEMSTRUCT*)lParam;
      drawGamepadTestArea(dis->hDC,&dis->rcItem);
      return TRUE;
    }
    break;

  case WM_TIMER:
    InvalidateRect(GetDlgItem(hwnd,IDC_GAMEPAD_TESTAREA),NULL,FALSE);
    if (gamepadAssignTarget != GA_NONE && diJoystick != NULL)
    {
      diJoystick->Poll();
      DIJOYSTATE2 js;
      ZeroMemory(&js,sizeof js);
      if (SUCCEEDED(diJoystick->GetDeviceState(sizeof js,&js)))
      {
        int found = gamepadAssignPoll(js);
        if (found >= 0)
        {
          switch (gamepadAssignTarget)
          {
          case GA_CONFIRM: gamepadButtonConfirm = found; break;
          case GA_CANCEL: gamepadButtonCancel = found; break;
          case GA_RUN: gamepadButtonRun = found; break;
          default: break;
          }
          gamepadSaveConfig();
          gamepadAssignTarget = GA_NONE;
          updateGamepadStatusLabels(hwnd);
        }
      }
    }
    return TRUE;

  case WM_COMMAND:
    switch (LOWORD(wParam))
    {
    case IDC_GAMEPAD_CONFIRM_ASSIGN:
      if (diJoystick != NULL)
      {
        gamepadAssignBegin(GA_CONFIRM);
        SetWindowText(GetDlgItem(hwnd,IDC_GAMEPAD_CONFIRM_STATUS),LS(IDS_UI_GAMEPAD_WAITING));
      }
      return TRUE;
    case IDC_GAMEPAD_CANCEL_ASSIGN:
      if (diJoystick != NULL)
      {
        gamepadAssignBegin(GA_CANCEL);
        SetWindowText(GetDlgItem(hwnd,IDC_GAMEPAD_CANCEL_STATUS),LS(IDS_UI_GAMEPAD_WAITING));
      }
      return TRUE;
    case IDC_GAMEPAD_RUN_ASSIGN:
      if (diJoystick != NULL)
      {
        gamepadAssignBegin(GA_RUN);
        SetWindowText(GetDlgItem(hwnd,IDC_GAMEPAD_RUN_STATUS),LS(IDS_UI_GAMEPAD_WAITING));
      }
      return TRUE;
    case IDC_GAMEPAD_RESET:
      gamepadButtonConfirm = -1;
      gamepadButtonCancel = -1;
      gamepadButtonRun = -1;
      gamepadAssignTarget = GA_NONE;
      gamepadSaveConfig();
      updateGamepadStatusLabels(hwnd);
      return TRUE;
    case IDOK:
    case IDCANCEL:
      KillTimer(hwnd,1);
      EndDialog(hwnd,wParam);
      return TRUE;
    }
    break;

  case WM_DESTROY:
    KillTimer(hwnd,1);
    break;
  }
  return FALSE;
}

// Opens the gamepad configuration dialog (reachable from the window's
// system menu, see WM_SYSCOMMAND in wndProc). Live-tests the connected
// DirectInput device and lets the player (re-)assign the Confirm/
// Cancel/Run roles by pressing the physical button/trigger they want,
// or clear them via Reset -- unlike XInput, DirectInput has no
// standard button numbering, so this has to stay available for
// re-calibration rather than being a one-shot startup guess.
void showGamepadConfigDialog(HINSTANCE instance, HWND parent)
{
  gamepadAssignTarget = GA_NONE;
  gamepadConfigDialogOpen = true;
  showDialog(instance,IDD_GAMEPAD,parent,gamepadConfigDlgProc);
  gamepadConfigDialogOpen = false;
}

// Called by Windows with any messages for the window
LRESULT CALLBACK wndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg)
  {
  case WM_DESTROY:
    // If the window is closed, shut down the program
    PostQuitMessage(0);
    break;

  case WM_ERASEBKGND:
    return 1;
  case WM_PAINT:
    {
      // Copy the drawing bitmap to the painting device context
      PAINTSTRUCT ps;
      BeginPaint(wnd,&ps);
      BitBlt(ps.hdc,0,0,winSize.cx,winSize.cy,drawDC,0,0,SRCCOPY);
      EndPaint(wnd,&ps);
    }
    return 0;

  case WM_CHAR:
    if (wParam == 13)
      wParam = 10;
    inputKeys.push_back(wParam);
    return 0;
  case WM_KEYDOWN:
    switch (wParam)
    {
    case VK_LEFT:
      inputKeys.push_back('4');
      return 0;
    case VK_RIGHT:
      inputKeys.push_back('6');
      return 0;
    case VK_UP:
      inputKeys.push_back('8');
      return 0;
    case VK_DOWN:
      inputKeys.push_back('2');
      return 0;
    }
    break;

  case WM_TIMER:
    if (wParam == 2)
    {
      pollGamepad();
    }
    else
    {
      // Toggle and redraw the input cursor
      cursorSolid = !cursorSolid;
      if (cursorOn)
        drawCursor();
    }
    break;

  case WM_SYSCOMMAND:
    if ((wParam & 0xFFF0) == ID_SYSMENU_GAMEPAD)
    {
      showGamepadConfigDialog(appInstance,wnd);
      return 0;
    }
    break;
  }

  // Use the default Windows processing
  return DefWindowProc(wnd,msg,wParam,lParam);
}

// Called by Windows with any messages for the saved game list control
LONG_PTR oldSavedListProc = 0;
LRESULT CALLBACK savedListProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  LRESULT result = CallWindowProc((WNDPROC)oldSavedListProc,wnd,msg,wParam,lParam);

  // If the user releases the mouse over a blank area, clear the selection
  if (msg == WM_LBUTTONUP)
  {
    bool found = false;
    int count = SendMessage(wnd,LB_GETCOUNT,0,0);
    for (int i = 0; i < count; i++)
    {
      RECT r;
      if (SendMessage(wnd,LB_GETITEMRECT,i,(LPARAM)&r) != LB_ERR)
      {
        POINT p = { LOWORD(lParam), HIWORD(lParam) };
        if (PtInRect(&r,p))
          found = true;
      }
    }
    if (found == false)
      PostMessage(wnd,LB_SETCURSEL,-1,0);
  }

  return result;
}

// Switches which LANGUAGE block in WinOmega.rc subsequent resource lookups
// (LoadStringA via LS(), DialogBox, etc.) resolve to, by adjusting the
// current thread's locale -- the standard mechanism the Win32 resource
// loader consults when a binary has more than one LANGUAGE block for the
// same resource. A lang of 0 restores whatever locale was in effect
// before any override (ie. the OS default, the prior behaviour).
void applyUILanguage(DWORD lang)
{
  SetThreadLocale(lang != 0 ? MAKELCID(lang,SORT_DEFAULT) : originalLocale);
}

// Lets portable C code (file.c) pick a translated copy of a plain-text
// file (help*.txt, motd.txt, ...) when one exists, without needing to
// know anything about LANGUAGE blocks or the registry setting itself --
// it just reflects whatever applyUILanguage() last put in effect.
extern "C" int isJapaneseUILanguage()
{
  return PRIMARYLANGID(LANGIDFROMLCID(GetThreadLocale())) == LANG_JAPANESE;
}

// Called by Windows with any messages for the setup dialog
INT_PTR CALLBACK dlgProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg)
  {
  case WM_INITDIALOG:
    {
      // Initialize the graphics control
      SendMessage(GetDlgItem(wnd,IDC_GRAPHICS),BM_SETCHECK,graphics ? BST_CHECKED : BST_UNCHECKED,0);

      // Initialize the font face control
      HWND fontFaceCtrl = GetDlgItem(wnd,IDC_FONTFACE);
      for (std::set<std::string>::iterator it = fontNames.begin(); it != fontNames.end(); ++it)
        SendMessage(fontFaceCtrl,CB_ADDSTRING,0,(LPARAM)it->c_str());
      SendMessage(fontFaceCtrl,CB_SELECTSTRING,-1,(LPARAM)fontSetup.lfFaceName);

      // Initialize the font size control
      HWND fontSizeCtrl = GetDlgItem(wnd,IDC_FONTSIZE);
      char fontSizeStr[16];
      for (int i = 8; i <= 18; i++)
      {
        itoa(i,fontSizeStr,10);
        SendMessage(fontSizeCtrl,CB_ADDSTRING,0,(LPARAM)fontSizeStr);
      }
      itoa(fontSize,fontSizeStr,10);
      SendMessage(fontSizeCtrl,CB_SELECTSTRING,-1,(LPARAM)fontSizeStr);

      // Initialize the language control. Language names are shown in
      // their own language regardless of the current UI language, which
      // is why these aren't pulled from LS().
      HWND languageCtrl = GetDlgItem(wnd,IDC_LANGUAGE);
      SendMessage(languageCtrl,CB_ADDSTRING,0,(LPARAM)"(System default)");
      SendMessage(languageCtrl,CB_ADDSTRING,0,(LPARAM)"English");
      SendMessage(languageCtrl,CB_ADDSTRING,0,(LPARAM)"\x93\xFA\x96\x7B\x8C\xEA");
      int langSel = 0;
      if (uiLanguage == MAKELANGID(LANG_ENGLISH,SUBLANG_ENGLISH_UK))
        langSel = 1;
      else if (uiLanguage == MAKELANGID(LANG_JAPANESE,SUBLANG_JAPANESE_JAPAN))
        langSel = 2;
      SendMessage(languageCtrl,CB_SETCURSEL,langSel,0);

      // Initialize the "remember this selection" checkbox
      SendMessage(GetDlgItem(wnd,IDC_SAVE_LANGUAGE),BM_SETCHECK,saveLanguageChoice ? BST_CHECKED : BST_UNCHECKED,0);

      // Initialize the saved games control
      HWND savedCtrl = GetDlgItem(wnd,IDC_SAVED);
      oldSavedListProc = SetWindowLongPtr(savedCtrl,GWLP_WNDPROC,(LONG_PTR)savedListProc);
      WIN32_FIND_DATA fd;
      HANDLE find = FindFirstFile("Save\\*.*",&fd);
      if (find != INVALID_HANDLE_VALUE)
      {
        do
        {
          if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
            SendMessage(savedCtrl,LB_ADDSTRING,0,(LPARAM)fd.cFileName);
        }
        while (FindNextFile(find,&fd) != 0);
      }

      // Set the dialog's icon
      SendMessage(wnd,WM_SETICON,ICON_BIG,(LPARAM)icon);
      SendMessage(wnd,WM_SETICON,ICON_SMALL,(LPARAM)icon);

      // Centre the dialog
      RECT dlgRect;
      GetWindowRect(wnd,&dlgRect);
      int dlgWidth = dlgRect.right-dlgRect.left;
      int dlgHeight = dlgRect.bottom-dlgRect.top;
      SetWindowPos(wnd,HWND_TOP,
        (workArea.right+workArea.left-dlgWidth)/2,(workArea.bottom+workArea.top-dlgHeight)/2,
        0,0,SWP_NOOWNERZORDER|SWP_NOSIZE|SWP_NOZORDER);
    }
    return TRUE;

  case WM_COMMAND:
    switch (LOWORD(wParam))
    {
    case IDOK:
    case IDCANCEL:
      {
        // Read in the user's display settings
        graphics = (SendMessage(GetDlgItem(wnd,IDC_GRAPHICS),BM_GETCHECK,0,0) == BST_CHECKED);
        GetWindowText(GetDlgItem(wnd,IDC_FONTFACE),fontSetup.lfFaceName,LF_FACESIZE);
        char fontSizeStr[16];
        GetWindowText(GetDlgItem(wnd,IDC_FONTSIZE),fontSizeStr,16);
        fontSize = atoi(fontSizeStr);
        if (fontSize < 6)
          fontSize = 6;

        // Read in the user's language selection
        int langSel = SendMessage(GetDlgItem(wnd,IDC_LANGUAGE),CB_GETCURSEL,0,0);
        switch (langSel)
        {
        case 1: uiLanguage = MAKELANGID(LANG_ENGLISH,SUBLANG_ENGLISH_UK); break;
        case 2: uiLanguage = MAKELANGID(LANG_JAPANESE,SUBLANG_JAPANESE_JAPAN); break;
        default: uiLanguage = 0; break;
        }
        applyUILanguage(uiLanguage);

        // Read whether the language selection should be remembered, and
        // persist that choice itself so the checkbox stays in the state
        // the user left it in
        saveLanguageChoice = (SendMessage(GetDlgItem(wnd,IDC_SAVE_LANGUAGE),BM_GETCHECK,0,0) == BST_CHECKED);
        DWORD regSaveLanguage = saveLanguageChoice ? 1 : 0;
        RegSetValueEx(settings,"Save Language",0,REG_DWORD,(BYTE*)&regSaveLanguage,sizeof regSaveLanguage);

        // Save the user's display settings
        DWORD regGraphics = graphics ? 1 : 0;
        RegSetValueEx(settings,"Graphics",0,REG_DWORD,(BYTE*)&regGraphics,sizeof regGraphics);
        RegSetValueEx(settings,"Font Name",0,REG_SZ,(BYTE*)fontSetup.lfFaceName,strlen(fontSetup.lfFaceName)+1);
        DWORD regFontSize = fontSize;
        RegSetValueEx(settings,"Font Size",0,REG_DWORD,(BYTE*)&regFontSize,sizeof regFontSize);
        if (saveLanguageChoice)
          RegSetValueEx(settings,"Language",0,REG_DWORD,(BYTE*)&uiLanguage,sizeof uiLanguage);
        else
          RegDeleteValue(settings,"Language");

        // Read in the user's saved game selection
        int sel = SendMessage(GetDlgItem(wnd,IDC_SAVED),LB_GETCURSEL,0,0);
        if (sel != LB_ERR)
        {
          char saved[_MAX_PATH];
          SendMessage(GetDlgItem(wnd,IDC_SAVED),LB_GETTEXT,sel,(LPARAM)saved);
          sprintf(OmegaSave,"Save\\%s",saved);
        }
      }

      EndDialog(wnd,wParam);
      return TRUE;
    }
    break;
  }
  return FALSE;
}

// Called by Windows when enumerating fronts
int CALLBACK fontProc(ENUMLOGFONTEX* font, NEWTEXTMETRICEX* metric, DWORD fontType, LPARAM)
{
  bool allow = false;
  if (fontType & TRUETYPE_FONTTYPE)
    allow = true;
  else if (!(fontType & RASTER_FONTTYPE))
    allow = ((metric->ntmTm.ntmFlags & NTM_PS_OPENTYPE|NTM_TT_OPENTYPE|NTM_TYPE1) != 0);

  if (allow && (font->elfLogFont.lfPitchAndFamily & FIXED_PITCH))
  {
    if (font->elfLogFont.lfFaceName[0] != '@')
      fontNames.insert(font->elfLogFont.lfFaceName);
  }
  return 1;
}

// Entry point into the program
int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int show)
{
  appInstance = instance;

  // Don't display horrible old error dialogs
  SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOOPENFILEERRORBOX);

  // Remember the OS-determined thread locale so an explicit language
  // override can later be reverted back to "system default"
  originalLocale = GetThreadLocale();

  // Initialize COM and controls
  CoInitialize(NULL);
  InitCommonControls();

  // Get the path to Omega's library files
  GetModuleFileName(0,OMEGALIB,_MAX_PATH);
  char* pathLib = strrchr(OMEGALIB,'\\');
  if (pathLib == NULL)
    fatal(LS(IDS_UI_OMEGALIB_PATH));
  strcpy(pathLib+1,"OmegaLib\\");

  // Create the directory for Omega to store files in
  char omegaDir[_MAX_PATH];
  if (FAILED(SHGetFolderPath(0,CSIDL_APPDATA,0,SHGFP_TYPE_CURRENT,omegaDir)))
    fatal(LS(IDS_UI_APPDATA_PATH));
  strcat(omegaDir,"\\Omega");
  CreateDirectory(omegaDir,NULL);
  if (SetCurrentDirectory(omegaDir) == 0)
    fatal(LS(IDS_UI_SET_CURRENT_DIR));
  copyLibFile("omega.hi");
  copyLibFile("omega.log");
  strcat(omegaDir,"\\Save");
  CreateDirectory(omegaDir,NULL);

  // Get the desktop window
  HWND desktopWnd = GetDesktopWindow();
  HDC desktopDC = GetDC(desktopWnd);

  // Get the display rectange of the main monitor
  HMONITOR mon = MonitorFromWindow(desktopWnd,MONITOR_DEFAULTTOPRIMARY);
  MONITORINFO mi;
  ZeroMemory(&mi,sizeof mi);
  mi.cbSize = sizeof mi;
  GetMonitorInfo(mon,&mi);
  workArea = mi.rcWork;
  int scrWidth = workArea.right-workArea.left;
  int scrHeight = workArea.bottom-workArea.top;

  // Get all fixed width fonts. The charset must match the system's default
  // ANSI codepage (CP_ACP), since all text in this app -- resource strings,
  // MultiByteToWideChar(CP_ACP,...) calls, etc -- is encoded that way. On a
  // Japanese-locale system (CP932) this picks Shift-JIS capable fonts so
  // double-byte characters get shaped correctly instead of rendering as tofu.
  ZeroMemory(&fontSetup,sizeof fontSetup);
  fontSetup.lfCharSet = (GetACP() == 932) ? SHIFTJIS_CHARSET : ANSI_CHARSET;
  EnumFontFamiliesEx(desktopDC,&fontSetup,(FONTENUMPROC)fontProc,0,0);

  // Get the Omega icon
  icon = LoadIcon(instance,MAKEINTRESOURCE(IDI_OMEGA));

  // Choose the initial font name. On a Japanese system EnumFontFamiliesEx
  // (ANSI) reports the localized face name for "MS Gothic" as Shift-JIS
  // bytes (full-width "MS Gothic"), not the English name, so both are checked.
  if (fontSetup.lfCharSet == SHIFTJIS_CHARSET && fontNames.count("\x82\x6c\x82\x72\x20\x83\x53\x83\x56\x83\x62\x83\x4e") == 1)
    strcpy(fontSetup.lfFaceName,"\x82\x6c\x82\x72\x20\x83\x53\x83\x56\x83\x62\x83\x4e");
  else if (fontSetup.lfCharSet == SHIFTJIS_CHARSET && fontNames.count("MS Gothic") == 1)
    strcpy(fontSetup.lfFaceName,"MS Gothic");
  else if (fontSetup.lfCharSet == SHIFTJIS_CHARSET && fontNames.count("MS UI Gothic") == 1)
    strcpy(fontSetup.lfFaceName,"MS UI Gothic");
  else if (fontNames.count("Consolas") == 1)
    strcpy(fontSetup.lfFaceName,"Consolas");
  else if (fontNames.count("Lucida Console") == 1)
    strcpy(fontSetup.lfFaceName,"Lucida Console");
  else if ((fontNames.size() == 0) || (fontNames.count("Courier New") == 1))
    strcpy(fontSetup.lfFaceName,"Courier New");
  else
    strcpy(fontSetup.lfFaceName,fontNames.begin()->c_str());

  // Open the registry key that settings are stored under
  if (RegCreateKeyEx(HKEY_CURRENT_USER,"Software\\David Kinder\\Omega",0,NULL,
    REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&settings,NULL) != ERROR_SUCCESS)
    fatal(LS(IDS_UI_SETTINGS_REGISTRY));

  // Read the user's settings, if present
  BYTE setData[256];
  DWORD setLength = 256;
  DWORD setType = 0;
  if (RegQueryValueEx(settings,"Graphics",NULL,&setType,setData,&setLength) == ERROR_SUCCESS)
  {
    if (setType == REG_DWORD)
      graphics = (*((DWORD*)setData) != 0);
  }
  setLength = 256;
  if (RegQueryValueEx(settings,"Font Name",NULL,&setType,setData,&setLength) == ERROR_SUCCESS)
  {
    if (setType == REG_SZ)
      strcpy(fontSetup.lfFaceName,(char*)setData);
  }
  setLength = 256;
  if (RegQueryValueEx(settings,"Font Size",NULL,&setType,setData,&setLength) == ERROR_SUCCESS)
  {
    if (setType == REG_DWORD)
      fontSize = *((DWORD*)setData);
  }
  setLength = 256;
  if (RegQueryValueEx(settings,"Save Language",NULL,&setType,setData,&setLength) == ERROR_SUCCESS)
  {
    if (setType == REG_DWORD)
      saveLanguageChoice = (*((DWORD*)setData) != 0);
  }
  setLength = 256;
  if (RegQueryValueEx(settings,"Language",NULL,&setType,setData,&setLength) == ERROR_SUCCESS)
  {
    if (setType == REG_DWORD)
      uiLanguage = *((DWORD*)setData);
  }
  applyUILanguage(uiLanguage);

  // Read the DirectInput gamepad button calibration, if present (see
  // showGamepadConfigDialog())
  setLength = 256;
  if (RegQueryValueEx(settings,"Gamepad Confirm",NULL,&setType,setData,&setLength) == ERROR_SUCCESS && setType == REG_DWORD)
    gamepadButtonConfirm = *((int*)setData);
  setLength = 256;
  if (RegQueryValueEx(settings,"Gamepad Cancel",NULL,&setType,setData,&setLength) == ERROR_SUCCESS && setType == REG_DWORD)
    gamepadButtonCancel = *((int*)setData);
  setLength = 256;
  if (RegQueryValueEx(settings,"Gamepad Run",NULL,&setType,setData,&setLength) == ERROR_SUCCESS && setType == REG_DWORD)
    gamepadButtonRun = *((int*)setData);

  // Show the setup dialog
  if (showDialog(instance,IDD_SETUP,0,dlgProc) != IDOK)
    return 0;

  // Create a font
  int logY = GetDeviceCaps(desktopDC,LOGPIXELSY);
  fontSetup.lfHeight = -MulDiv(fontSize,logY,72);
  font = CreateFontIndirect(&fontSetup);
  HGDIOBJ oldFont = SelectObject(desktopDC,font);
  GetTextMetrics(desktopDC,&fontMetrics);
  SelectObject(desktopDC,oldFont);

  // Make the display higher, if possible, and set the window size
  int maxLines = (scrHeight*85)/(fontMetrics.tmHeight*100);
  if (maxLines > 32)
    maxLines = 32;
  if (maxLines > LINES)
    LINES = maxLines;
  winSize.cx = fontMetrics.tmAveCharWidth*COLS;
  winSize.cy = fontMetrics.tmHeight*LINES;

  // Create the display bitmap
  drawDC = CreateCompatibleDC(desktopDC);
  BITMAPINFO bi;
  ZeroMemory(&bi,sizeof bi);
  bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bi.bmiHeader.biWidth = winSize.cx;
  bi.bmiHeader.biHeight = -1*winSize.cy;
  bi.bmiHeader.biPlanes = 1;
  bi.bmiHeader.biBitCount = 24;
  bi.bmiHeader.biCompression = BI_RGB;
  void* drawBits;
  drawBmp = CreateDIBSection(drawDC,&bi,DIB_RGB_COLORS,&drawBits,0,0);
  if (drawBmp == 0)
    fatal(LS(IDS_UI_CREATE_BITMAP));

  // Initialize the display bitmap
  SelectObject(drawDC,drawBmp);
  SelectObject(drawDC,font);
  SetBkMode(drawDC,OPAQUE);
  drawRect(0,0,winSize.cx,winSize.cy,RGB(0,0,0));

  // Free the desktop window
  ReleaseDC(desktopWnd,desktopDC);
  desktopDC = 0;

  if (graphics)
  {
    // Load the graphics bitmap
    char bitmapPath[_MAX_PATH];
    GetModuleFileName(0,bitmapPath,_MAX_PATH);
    char* bitmapFile = strrchr(bitmapPath,'\\');
    if (bitmapFile == NULL)
      fatal(LS(IDS_UI_BITMAP_PATH));
    strcpy(bitmapFile+1,"32x32.bmp");
    gfxBmp = (HBITMAP)LoadImage(NULL,bitmapPath,IMAGE_BITMAP,0,0,
      LR_CREATEDIBSECTION|LR_LOADFROMFILE);
    if (gfxBmp == 0)
    {
      MessageBox(0,LS(IDS_UI_GRAPHICS_NOT_LOADED),"Omega",MB_ICONERROR|MB_OK);
      graphics = false;
    }

    // Create a device context for the graphics
    gfxDC = CreateCompatibleDC(drawDC);
    SelectObject(gfxDC,gfxBmp);

    // Initialize the graphics tile mapping
    for (int i = 0; i < sizeof gfxMapData / sizeof gfxMapData[0]; i++)
    {
      assert(gfxMap.count(gfxMapData[i][0]) == 0);
      POINT p = { gfxMapData[i][1],gfxMapData[i][2] };
      gfxMap[gfxMapData[i][0]] = p;
    }
  }

  // Register window class
  WNDCLASSEX wc;
  ZeroMemory(&wc,sizeof wc);
  wc.cbSize = sizeof wc;
  wc.style = CS_HREDRAW|CS_VREDRAW;
  wc.lpfnWndProc = wndProc;
  wc.hInstance = instance;
  wc.hIcon = icon;
  wc.hCursor = LoadCursor(NULL,IDC_ARROW);
  wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
  wc.lpszClassName = "Omega";
  RegisterClassEx(&wc);

  // Create the window
  wnd = CreateWindowEx(WS_EX_CLIENTEDGE,"Omega","Omega",
    WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_CLIPCHILDREN|WS_MINIMIZEBOX,0,0,100,100,0,0,instance,NULL);
  if (wnd == 0)
    fatal(LS(IDS_UI_CREATE_WINDOW));

  // Get the size of the window frame
  RECT rectW,rectC;
  GetWindowRect(wnd,&rectW);
  GetClientRect(wnd,&rectC);

  // Centre and show the window
  int w = winSize.cx+(rectW.right-rectW.left-rectC.right);
  int h = winSize.cy+(rectW.bottom-rectW.top-rectC.bottom);
  int x = workArea.left+((scrWidth-w)/2);
  int y = workArea.top+((scrHeight-h)/2);
  MoveWindow(wnd,x,y,w,h,FALSE);
  ShowWindow(wnd,show);

  // Start a timer to flash the cursor
  SetTimer(wnd,1,GetCaretBlinkTime(),NULL);

  // Start a timer to poll for gamepad input
  initXInput();
  initDirectInput(instance,wnd);

  // Add a "Configure Gamepad..." item to the window's system menu (the
  // menu that opens from the title bar icon or Alt+Space) so the
  // DirectInput button calibration (see showGamepadConfigDialog()) can
  // be reopened any time during play, not just automatically on first
  // run -- DirectInput's button numbering isn't standardized the way
  // XInput's is, so this needs to stay reachable/re-doable per
  // controller rather than being a one-shot startup prompt.
  HMENU sysMenu = GetSystemMenu(wnd,FALSE);
  if (sysMenu != NULL)
  {
    AppendMenu(sysMenu,MF_SEPARATOR,0,NULL);
    AppendMenu(sysMenu,MF_STRING,ID_SYSMENU_GAMEPAD,LS(IDS_UI_GAMEPAD_MENU_ITEM));
  }

  // 16ms (~60Hz) rather than a rounder 50ms: the debounce logic in
  // pushGamepadDirection()/GamepadDebounce requires a value to be seen
  // for a couple of consecutive polls before it's trusted, so a slower
  // poll rate directly raises the minimum press duration needed to
  // register at all -- at 50ms that was ~100ms, long enough to eat
  // genuine quick taps. At 16ms the same tick counts only cost ~32-48ms.
  SetTimer(wnd,2,16,NULL);

  // Run Omega
  char* argv[2] = { "omega",OmegaSave };
  int argc = (OmegaSave[0] == '\0') ? 1 : 2;
  return main(argc,argv);
}

// Curses functions

int impl_wprintw(WINDOW *win, const char *fmt, va_list list)
{
  char buffer[256];
  vsprintf(buffer,fmt,list);

  int len = (int)strlen(buffer);
  for (int i = 0; i < len; i++)
    waddch(win,*(buffer+i));
  return len;
}

extern "C" int clear(void)
{
  return werase(stdscr);
}

extern "C" int crmode(void)
{
  if (graphics)
  {
    // Adjust what Omega thinks is the screen height so that
    // the larger size of graphics tiles is taken into account
    if (gfxSize.cy > fontMetrics.tmHeight)
    {
      int height = fontMetrics.tmHeight*(Levelw->_maxy+1);
      ScreenLength = height/gfxSize.cy;
    }
  }
  return 0;
}

extern "C" int endwin(void)
{
  // Do nothing
  return 0;
}

extern "C" WINDOW *initscr(void)
{
  // Set up default windows
  stdscr = newwin(LINES,COLS,0,0);
  curscr = stdscr;
  return stdscr;
}

extern "C" int move(int y, int x)
{
  return wmove(stdscr,y,x);
}

extern "C" WINDOW *newwin(int nlines, int ncols, int begin_y, int begin_x)
{
  WINDOW *win = (WINDOW *)calloc(1,sizeof(WINDOW));
  win->_cury = 0;
  win->_curx = 0;
  win->_maxy = nlines-1;
  win->_maxx = ncols-1;
  win->_offy = begin_y;
  win->_offx = begin_x;
  win->_text = (int*)malloc(ncols*nlines*sizeof(int));
  win->_line = (int*)malloc(nlines*sizeof(int));
  wattrset(win,COL_WHITE>>8);
  werase(win);
  return win;
}

extern "C" int noecho(void)
{
  // Do nothing
  return 0;
}

extern "C" int printw(const char *fmt, ...)
{
  va_list arg;
  va_start(arg,fmt);
  int n = impl_wprintw(stdscr,fmt,arg);
  va_end(arg);
  return n;
}

extern "C" int refresh(void)
{
  return wrefresh(stdscr);
}

extern "C" int scrollok(WINDOW *win, int bf)
{
  // Do nothing
  return 0;
}

extern "C" int touchwin(WINDOW *win)
{
  for (int i = 0; i <= win->_maxy; i++)
    win->_line[i] = 1;
  return 0;
}

extern "C" int waddch(WINDOW *win, const char ch)
{
  switch (ch)
  {
  case '\n':
    win->_curx = 0;
    if (win->_cury < win->_maxy)
      win->_cury++;
    break;
  default:
    // ch must be treated as unsigned here: isprint() with a negative
    // value other than EOF is undefined behaviour, and Shift-JIS
    // lead/trail bytes (0x80-0xFF) are negative as a signed char. The
    // sign-extension of a negative ch into the attribute bits of
    // ch+(win->_attr<<8) was a second, related bug, also fixed by the
    // (unsigned char) cast below.
    //
    // isprint() itself isn't multibyte-aware: nothing here ever calls
    // setlocale(), so it runs under the default "C" locale, where every
    // byte 0x80-0xFF is classified as non-printable by definition. That
    // silently dropped every Shift-JIS lead/trail byte instead of
    // storing it. Bytes with the high bit set are always a DBCS lead or
    // trail byte in this codebase (only Shift-JIS text is ever handled),
    // so let them through unconditionally instead of asking isprint().
    if (((unsigned char)ch >= 0x80) || isprint((unsigned char)ch))
    {
      win->_text[(win->_cury*(win->_maxx+1))+win->_curx] = (unsigned char)ch+(win->_attr<<8);
      win->_line[win->_cury] = 1;
      if (win->_curx < win->_maxx)
        win->_curx++;
    }
    break;
  }
  return 0;
}

extern "C" int waddstr(WINDOW *win, const char *str)
{
  return wprintw(win,str);
}

extern "C" int wattrset(WINDOW *win, int attrs)
{
  win->_attr = attrs;
  return 0;
}

extern "C" int werase(WINDOW *win)
{
  for (int i = 0; i < (win->_maxx+1)*(win->_maxy+1); i++)
    win->_text[i] = SPACE;
  for (int i = 0; i <= win->_maxy; i++)
    win->_line[i] = 1;
  win->_curx = 0;
  win->_cury = 0;
  return 0;
}

extern "C" int wgetch(WINDOW *win)
{
  // Turn the cursor on
  if (win != Levelw)
  {
    cursorOn = true;
    cursorSolid = false;
    cursorPos.x = fontMetrics.tmAveCharWidth*(win->_offx+win->_curx);
    cursorPos.y = fontMetrics.tmHeight*(win->_offy+win->_cury);
  }

  // Redraw the display
  InvalidateRect(wnd,NULL,FALSE);

  // Pump messages
  while (inputKeys.size() == 0)
  {
    // While there are messages, process them
    MSG msg;
    while (PeekMessage(&msg,0,0,0,PM_REMOVE))
    {
      // If a quit message has been received, exit the program
      if (msg.message == WM_QUIT)
        exit(0);
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
    if (inputKeys.size() == 0)
      WaitMessage();
  }

  // Turn the cursor off
  cursorSolid = false;
  if (cursorOn)
    drawCursor();
  cursorOn = false;

  int c = inputKeys.front();
  inputKeys.pop_front();
  return c;
}

extern "C" int wmove(WINDOW *win, int y, int x)
{
  if (y > win->_maxy)
    y = win->_maxy;
  if (x > win->_maxx)
    x = win->_maxx;
  win->_cury = y;
  win->_curx = x;
  return 0;
}

extern "C" int wprintw(WINDOW *win, const char *fmt, ...)
{
  va_list arg;
  va_start(arg,fmt);
  int n = impl_wprintw(win,fmt,arg);
  va_end(arg);
  return n;
}

extern "C" int wrefresh(WINDOW *win)
{
  if ((win == stdscr) || (win == Menuw))
  {
    touchwin(Msgw);
    touchwin(Msg1w);
    touchwin(Msg2w);
    touchwin(Msg3w);
    touchwin(Levelw);
    touchwin(Timew);
    touchwin(Flagw);
    touchwin(Dataw);
    touchwin(Locw);
    touchwin(Morew);
    touchwin(Phasew);
    touchwin(Comwin);
    touchwin(Menuw); 
    touchwin(stdscr);
  }

  if (graphics && (win == Levelw))
  {
    // Get the size of the graphics window in pixels
    SIZE gfxWinPixels;
    gfxWinPixels.cx = fontMetrics.tmAveCharWidth*(win->_maxx+1);
    gfxWinPixels.cy = fontMetrics.tmHeight*(win->_maxy+1);

    // Get the size of the graphics window in graphics tiles
    SIZE gfxWinTiles;
    gfxWinTiles.cx = (gfxSize.cx > fontMetrics.tmAveCharWidth) ? gfxWinPixels.cx/gfxSize.cx : win->_maxx+1;
    gfxWinTiles.cy = (gfxSize.cy > fontMetrics.tmHeight) ? gfxWinPixels.cy/gfxSize.cy : win->_maxy+1;

    // Get the resulting margin (if any) due to the difference in text and tile size
    SIZE gfxMargin;
    gfxMargin.cx = gfxWinPixels.cx - (gfxWinTiles.cx*gfxSize.cx);
    gfxMargin.cy = gfxWinPixels.cy - (gfxWinTiles.cy*gfxSize.cy);

    // Get the top-left corner to start drawing graphics at
    POINT gfxTopLeft;
    gfxTopLeft.x = (fontMetrics.tmAveCharWidth*win->_offx) + (gfxMargin.cx/2);
    gfxTopLeft.y = (fontMetrics.tmHeight*win->_offy) + (gfxMargin.cy/2);

    // Get the X offset to draw from
    int offsetX = win->_curx-(gfxWinTiles.cx/2);
    if (offsetX < 0)
      offsetX = 0;
    else if (offsetX > win->_maxx+1-gfxWinTiles.cx)
      offsetX = win->_maxx+1-gfxWinTiles.cx;

    // Draw the tiles
    drawRect(fontMetrics.tmAveCharWidth*win->_offx,fontMetrics.tmHeight*win->_offy,
      gfxWinPixels.cx,gfxWinPixels.cy,RGB(0,0,0));
    for (int y = 0; y < gfxWinTiles.cy; y++)
    {
      for (int x = 0; x < gfxWinTiles.cx; x++)
      {
        POINT tile = { -1,-1 };
        int c = win->_text[(y*(win->_maxx+1))+x+offsetX];

        // Map character to a tile for special cases
        if (Current_Environment != E_COUNTRYSIDE)
        {
          switch (c)
          {
          case 'o'|COL_WHITE: // snowball weapon
            tile.x = 69;
            tile.y = 1;
            break;
          case '!'|COL_RED: // incubus/succubus and satyr/nymph
            tile.x = 67;
            tile.y = 21;
            break;
          }
        }

        // If not a special case, look the tile up in the map
        if (tile.x == -1)
        {
          std::map<int,POINT>::const_iterator it = gfxMap.find(c);
          if (it != gfxMap.end())
            tile = it->second;
        }

        // Draw the tile
        if (tile.x != -1)
        {
          BitBlt(drawDC,
            gfxTopLeft.x+(x*gfxSize.cx),gfxTopLeft.y+(y*gfxSize.cy),gfxSize.cx,gfxSize.cy,
            gfxDC,tile.x*gfxSize.cx,tile.y*gfxSize.cy,SRCCOPY);
        }
        else
        {
          drawRect(gfxTopLeft.x+(x*gfxSize.cx),gfxTopLeft.y+(y*gfxSize.cy),
            gfxSize.cx,gfxSize.cy,RGB(255,0,0));
        }
      }
    }

    // Draw the target cursor
    if (target_on != 0)
    {
      int x = gfxTopLeft.x+((win->_curx-offsetX)*gfxSize.cx);
      int y = gfxTopLeft.y+(win->_cury*gfxSize.cy);
      RECT r = { x,y,x+gfxSize.cx,y+gfxSize.cy };
      FrameRect(drawDC,&r,(HBRUSH)GetStockObject(WHITE_BRUSH));
    }
  }
  else
  {
    for (int y = 0; y <= win->_maxy; y++)
    {
      // Check if the line needs updating
      if (win->_line[y] != 0)
      {
        int len = win->_maxx+1;
        if (len > 0)
        {
          // Get the text string for the line
          char* text = (char*)alloca(len);
          for (int x = 0; x < len; x++)
            text[x] = (char)(win->_text[(y*len)+x] & 0xFF);
          win->_line[y] = 0;

          // Step over the text string, drawing it whenever the character attribute changes
          int x1 = 0, x2 = 0;
          int attr1 = (win->_text[y*len] & 0xFF00)>>8;
          int attr2 = 0;
          while ((x1 < len) && (x2 <= len))
          {
            if (x2 < len)
              attr2 = (win->_text[(y*len)+x2] & 0xFF00)>>8;
            if ((x2 == len) || (attr1 != attr2))
            {
              SetTextColor(drawDC,palette[attr1&0x0F]);
              SetBkColor(drawDC,palette[(attr1&0xF0)>>4]);
              // Force every byte cell to advance by exactly one grid column
              // (two for the lead byte of a Shift-JIS DBCS pair, zero for its
              // trail byte), rather than letting TextOut space glyphs using
              // the font's own natural widths. Full-width Japanese glyphs are
              // rarely exactly 2x the ASCII advance, so without this the
              // fixed-width grid model (1 byte = 1 waddch = 1 column) and the
              // actual pixel layout drift apart and characters overlap.
              int runLen = x2-x1;
              int* dx = (int*)alloca(runLen*sizeof(int));
              for (int i = 0; i < runLen; )
              {
                unsigned char c = (unsigned char)text[x1+i];
                bool isLead = ((c >= 0x81 && c <= 0x9F) || (c >= 0xE0 && c <= 0xFC))
                  && (i+1 < runLen);
                if (isLead)
                {
                  dx[i] = fontMetrics.tmAveCharWidth*2;
                  dx[i+1] = 0;
                  i += 2;
                }
                else
                {
                  dx[i] = fontMetrics.tmAveCharWidth;
                  i += 1;
                }
              }
              ExtTextOut(drawDC,
                fontMetrics.tmAveCharWidth*(win->_offx+x1),
                fontMetrics.tmHeight*(win->_offy+y),
                0,NULL,text+x1,runLen,dx);

              attr1 = attr2;
              x1 = x2;
            }
            else
              x2++;
          }
        }
      }
    }

    // Draw the target cursor
    if (((win == Levelw)) && (target_on != 0))
    {
      int x = fontMetrics.tmAveCharWidth*(win->_offx+win->_curx);
      int y = fontMetrics.tmHeight*(win->_offy+win->_cury);
      RECT r = { x,y,x+fontMetrics.tmAveCharWidth,y+fontMetrics.tmHeight };
      FrameRect(drawDC,&r,(HBRUSH)GetStockObject(WHITE_BRUSH));

      // Make sure the target cursor does not get left on screen
      win->_line[win->_cury] = 1;
    }
  }
  return 0;
}

extern "C" int wstandend(WINDOW *win)
{
  if (win != Levelw)
    win->_attr = (COL_BG_BLACK|COL_WHITE)>>8;
  return 0;
}

extern "C" int wstandout(WINDOW *win)
{
  if (win != Levelw)
    win->_attr = (COL_BG_WHITE|COL_BLACK)>>8;
  return 0;
}

extern "C" void impl_getyx(WINDOW* win, int* y, int* x)
{
  *y = win->_cury;
  *x = win->_curx;
}

extern "C" int impl_getch(void)
{
  return wgetch(stdscr);
}

// Unix utility functions

void impl_sleep(unsigned int milli)
{
  // Redraw the display
  InvalidateRect(wnd,NULL,FALSE);

  // Pump messages until past end time
  DWORD endTime = GetTickCount()+milli;
  while (GetTickCount() < endTime)
  {
    // While there are messages, process them
    MSG msg;
    while (PeekMessage(&msg,0,0,0,PM_REMOVE))
    {
      // If a quit message has been received, exit the program
      if (msg.message == WM_QUIT)
        exit(0);
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

    // Wait for a message or until past end time
    DWORD timeOut = endTime-GetTickCount();
    if (timeOut > 0)
      MsgWaitForMultipleObjects(0,NULL,FALSE,timeOut,QS_ALLINPUT);
  }
}

extern "C" unsigned int sleep(unsigned int seconds)
{
  impl_sleep(seconds*1000);
  return 0;
}

extern "C" unsigned int usleep(unsigned int useconds)
{
  impl_sleep(useconds/1000);
  return 0;
}

extern "C" char *getlogin(void)
{
  char *login = getenv("OMEGA_LOGIN");
  if (login == NULL)
    login = "pcuser";
  return login;
}

extern "C" int chown(const char*, short, short)
{
  // Do nothing
  return 0;
}

// Omega functions

extern "C" char* save_dir(char* name)
{
  static char path[_MAX_PATH];
  sprintf(path,"Save\\%s",name);
  return path;
}
