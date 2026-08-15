/* omega copyright (C) by Laurence Raphael Brothers, 1987,1988,1989 */
/* command3.c */

/* This file contains some more top level command functions
   called from command1.c */

#if !defined(MSDOS) && !defined(_WIN32)
#include <pwd.h>
#include <unistd.h>
#endif

#include "glob.h"
#include "date.h"

/* look at some spot */
void examine()
{
  pol ol;
  int x=Player.x,y=Player.y,drewmenu=FALSE;

  clearmsg();

  setgamestatus(SKIP_MONSTERS);
  mprint(LS(IDS_MSG_20529));
  setspot(&x,&y);
  if (inbounds(x,y)) {
    clearmsg();
    if (Current_Environment == E_COUNTRYSIDE) {
      if (! c_statusp(x, y, SEEN))
	print3(LS(IDS_MSG_20530));
      else {
	mprint(LS(IDS_MSG_20531));
	mprint(countryid(Country[x][y].current_terrain_type));
      }
    } 
    else if (! view_los_p(Player.x,Player.y,x,y))
      print3(LS(IDS_MSG_20532));
    else {
      clearmsg();
      if (Level->site[x][y].creature != NULL) 
	mprint(mstatus_string(Level->site[x][y].creature));
      else if ((Player.x == x) && (Player.y ==y)) 
	describe_player();
      if (loc_statusp(x,y,SECRET))
	print2(LS(IDS_MSG_20533));
      else
	switch (Level->site[x][y].locchar) {
	case SPACE: print2(LS(IDS_MSG_20534)); break;
	case PORTCULLIS: print2(LS(IDS_MSG_20535)); break;
	case ABYSS: print2(LS(IDS_MSG_20536)); break;
	case FLOOR:
	  if (Current_Dungeon == Current_Environment)
	    print2(LS(IDS_MSG_20537));
	  else print2(LS(IDS_MSG_20538));
	  break;
	case WALL:
	  if (Level->site[x][y].aux == 0)
	    print2(LS(IDS_MSG_20539));
	  else if (Level->site[x][y].aux < 10)
	    print2(LS(IDS_MSG_20540));
	  else if (Level->site[x][y].aux < 30)
	    print2(LS(IDS_MSG_20541));
	  else if (Level->site[x][y].aux < 50)
	    print2(LS(IDS_MSG_20542));
	  else if (Level->site[x][y].aux < 70)
	    print2(LS(IDS_MSG_20543));
	  else if (Level->site[x][y].aux < 90)
	    print2(LS(IDS_MSG_20544));
	  else if (Level->site[x][y].aux < 210) {
	    if (Current_Environment == E_CITY)
	      print2(LS(IDS_MSG_20545));
	    else print2(LS(IDS_MSG_20546));
	  }
	  else print2(LS(IDS_MSG_20547));
	  break;
	case RUBBLE:print2(LS(IDS_MSG_20548)); break;
	case SAFE:print2(LS(IDS_MSG_20549)); break;
	case CLOSED_DOOR:print2(LS(IDS_MSG_20550)); break;
	case OPEN_DOOR:print2(LS(IDS_MSG_20551)); break;
	case STATUE:print2(LS(IDS_MSG_20552)); break;
	case STAIRS_UP:print2(LS(IDS_MSG_20553)); break;
	case STAIRS_DOWN:print2(LS(IDS_MSG_20554)); break;
	case TRAP:print2(trapid(Level->site[x][y].p_locf)); break;
	case HEDGE:
	  if (Level->site[x][y].p_locf == L_EARTH_STATION)
	    print2(LS(IDS_MSG_20555));
	  else print2(LS(IDS_MSG_20556)); 
	  break;
	case LAVA:print2(LS(IDS_MSG_20557)); break;
	case LIFT:print2(LS(IDS_MSG_20558)); break;
	case ALTAR:print2(LS(IDS_MSG_20559)); break;
	case CHAIR:print2(LS(IDS_MSG_20560)); break;
	case WHIRLWIND:print2(LS(IDS_MSG_20561)); break;
	case WATER: 
	  if (Level->site[x][y].p_locf == L_WATER)
	    print2(LS(IDS_MSG_20562));
	  else if (Level->site[x][y].p_locf == L_CHAOS)
	    print2(LS(IDS_MSG_20563));
	  else if (Level->site[x][y].p_locf == L_WATER_STATION)
	    print2(LS(IDS_MSG_20564));
	  else print2(LS(IDS_MSG_20565));
	  break;
	case FIRE:print2(LS(IDS_MSG_20566)); break;
	default:print2(LS(IDS_MSG_20567)); break;
	}
      if ((ol = Level->site[x][y].things) != NULL && !loc_statusp(x,y,SECRET)) {
	if (ol->next == NULL)
	  print3(itemid(ol->thing));
	else {
	  drewmenu = TRUE;
	  menuclear();
	  menuprint("Things on floor:\n");
	  while (ol != NULL) {
	    menuprint("\n");
	    menuprint(itemid(ol->thing));
	    ol = ol->next;
	  }
	  showmenu();
	}
      }
      morewait();
      sign_print(x,y,TRUE);
    }
  }
  if (drewmenu) xredraw();
}

void help()
{
  char c;
  char filestr[260]; // _WIN32
#if !defined(MSDOS) && !defined(AMIGA) && !defined(_WIN32)
  FILE *in, *out;
  int n;
#endif

  clearmsg();
  print1(LS(IDS_MSG_20568));
  menuclear();
  menuprint("a: Overview\n");
  menuprint("b: Characters\n");
  menuprint("c: Inventories\n");
  menuprint("d: Movement\n");
  menuprint("e: Combat\n");
  menuprint("f: Bugs\n");
  menuprint("g: Magic\n");
  menuprint("h: The Countryside\n");
  menuprint("i: The Screen Display\n");
  menuprint("j: Saving and Restoring\n");
  menuprint("k: Options Settings\n");
  menuprint("l: Dungeon/City/Other Command List\n");
  menuprint("m: Countryside Command List\n");
#if !defined(MSDOS) && !defined(AMIGA) && !defined(_WIN32)
  menuprint("n: Everything\n");
#endif
  menuprint("ESCAPE: Forget the whole thing.");
  showmenu();
  do 
    c = (char) mcigetc();
#if defined(MSDOS) || defined(AMIGA) || defined(_WIN32)
  while ((c < 'a' || c > 'm') && c != ESCAPE);
  if (c != ESCAPE) {
    char helpname[32];
    sprintf(helpname, "help%d.txt", c+1-'a');
    omegalibFile(filestr, helpname);
    displayfile(filestr);
  }
#else
  while ((c < 'a' || c > 'n') && c != ESCAPE);
  if (c == 'n') {
    print1(LS(IDS_MSG_20569));
    nprint1(LS(IDS_MSG_20570));
    if (ynq1()=='y') {
      change_to_user_perms();
      out = checkfopen("omega.doc", "w");
      print2(LS(IDS_MSG_20571));
      for (n = 1; n <= 13; n++)
      {
	nprint2(LS(IDS_MSG_20060));
	sprintf(Str1, "%shelp%d.txt", Omegalib, n);
	in = checkfopen(Str1, "r");
	while (fgets(Str1, STRING_LEN, in))
	  fputs(Str1, out);
	fclose(in);
      }
      fclose(out);
      change_to_game_perms();
      nprint2(LS(IDS_MSG_20572));
    }
  }
  else if (c != ESCAPE) {
    sprintf(filestr, "%shelp%d.txt", Omegalib, c+1-'a');
    print1(LS(IDS_MSG_20573));
    do 
      c = (char) mcigetc();
    while ((c != 'd') && (c != 'c')&& (c!=ESCAPE));
    if (c == 'd')
      displayfile(filestr);
    else if (c == 'c') copyfile(filestr);
  }
#endif
  xredraw();
}

void version()
{
  setgamestatus(SKIP_MONSTERS);
  print3(VERSIONSTRING);
  nprint3(LS(IDS_MSG_20574));
  nprint3(LS(IDS_MSG_20575));
  nprint3(LAST_OMEGA_EDIT_DATE);
}

void fire()
{
  int index,x1,y1,x2,y2;
  pob obj;
  struct monster *m;

  clearmsg();

  print1(LS(IDS_MSG_20576));
  index = getitem(NULL_ITEM);
  if (index == ABORT)
    setgamestatus(SKIP_MONSTERS);
  else if (index == CASHVALUE) print3(LS(IDS_MSG_20577));
  else if (cursed(Player.possessions[index]) && 
	   Player.possessions[index]->used)
    print3(LS(IDS_MSG_20578));
  /* load a crossbow */
  else if ((Player.possessions[O_WEAPON_HAND] != NULL) &&  
	   (Player.possessions[O_WEAPON_HAND]->id == WEAPONID+27) && 
	   (Player.possessions[O_WEAPON_HAND]->aux != LOADED) &&
	   (Player.possessions[index]->id == WEAPONID+29)) {
	     mprint(LS(IDS_MSG_20579));
	     Player.possessions[O_WEAPON_HAND]->aux = LOADED;
	   }
  else {
    if (Player.possessions[index]->used) {
      Player.possessions[index]->used = FALSE;
      item_use(Player.possessions[index]);
    }
    obj = Player.possessions[index];
    x1 = x2 = Player.x;
    y1 = y2 = Player.y;
    setspot(&x2,&y2);
    if ((x2 == Player.x) && (y2 == Player.y)) 
      mprint(LS(IDS_MSG_20580));
    else {
      do_object_los(obj->objchar,&x1,&y1,x2,y2);
      if ((m=Level->site[x1][y1].creature) != NULL) {
	if (obj->dmg == 0) {
	  if (m->treasure > 0) { /* the monster can have treasure/objects */
	    mprint(LS(IDS_MSG_20581));
	    givemonster(m,split_item(1,obj));
	    conform_lost_objects(1,obj);
	  }
	  else {
	    mprint(LS(IDS_MSG_20582));
	    setgamestatus(SUPPRESS_PRINTING);
	    p_drop_at(x1,y1,1,obj);
	    resetgamestatus(SUPPRESS_PRINTING);
	    conform_lost_objects(1,obj);
	  }
	}
	else if (obj->aux == I_SCYTHE) {
	  mprint(LS(IDS_MSG_20583));
	  setgamestatus(SUPPRESS_PRINTING);
	  p_drop_at(x1,y1,1,obj);
	  resetgamestatus(SUPPRESS_PRINTING);
	  conform_lost_objects(1,obj);
	}
	else if (hitp(Player.hit,m->ac)) {/* ok already, hit the damn thing */
	  weapon_use(2*statmod(Player.str),obj,m);
	  if ((obj->id == WEAPONID+28 || obj->id == WEAPONID+29) &&
	    !random_range(4))
	    dispose_lost_objects(1,obj);
	  else {
	    setgamestatus(SUPPRESS_PRINTING);
	    p_drop_at(x1,y1,1,obj);
	    resetgamestatus(SUPPRESS_PRINTING);
	    conform_lost_objects(1,obj);
	  }
	}
	else {
	  mprint(LS(IDS_MSG_20584));
	  setgamestatus(SUPPRESS_PRINTING);
	  p_drop_at(x1,y1,1,obj);
	  resetgamestatus(SUPPRESS_PRINTING);
	  conform_lost_objects(1,obj);
	}
      }
      else {
	setgamestatus(SUPPRESS_PRINTING);
	p_drop_at(x1,y1,1,obj);
	resetgamestatus(SUPPRESS_PRINTING);
	conform_lost_objects(1,obj);
	plotspot(x1,y1,TRUE);
      }
    }
  }
}



void quit()
{
  clearmsg();
  change_to_game_perms();
  mprint(LS(IDS_MSG_20585));
  if (ynq()=='y') {
    if (Player.rank[ADEPT] == 0) display_quit();
    else display_bigwin();
#ifdef SAVE_LEVELS
    kill_all_levels();
#endif
    endgraf();
    exit(0);
  }
  else resetgamestatus(SKIP_MONSTERS);
}



/* rest in 10 second segments so if woken up by monster won't
die automatically.... */
void nap()
{
  static int naptime;
  if (gamestatusp(FAST_MOVE)) {
    if (naptime-- < 1) {
      clearmsg();
      mprint(LS(IDS_MSG_20586));
      resetgamestatus(FAST_MOVE);
      drawvision(Player.x,Player.y);
    }
  }
  else {
    clearmsg();
    mprint(LS(IDS_MSG_20587));
    naptime = (int) parsenum();
    if (naptime > 600) {
      print3(LS(IDS_MSG_20588));
      naptime = 3600;
    }
    else naptime *= 6;
    if (naptime > 1) {
      clearmsg();
      setgamestatus(FAST_MOVE);
      mprint(LS(IDS_MSG_20589));
    }
  }
}


void charid()
{
  char id;
  int countryside=FALSE;
  char cstr[80];

  clearmsg();
  mprint(LS(IDS_MSG_20590));
  id = mgetc();
  if (Current_Environment == E_COUNTRYSIDE) {
    countryside = TRUE;
    strcpy(cstr,countryid(id));
    if (strcmp(cstr,"I have no idea.")==0)
      countryside = FALSE;
    else mprint(cstr);
  }
  if (! countryside) {
    if ((id >= 'a' && id <= 'z') || (id >= 'A' && id <= 'Z') || id == '@')
      mprint(LS(IDS_MSG_20591));
    else switch(id) {
    case (SPACE&0xff):
      mprint(LS(IDS_MSG_20592));
      break;
    case (WALL&0xff):
      mprint(LS(IDS_MSG_20593));
      break;
    case (OPEN_DOOR&0xff):
      mprint(LS(IDS_MSG_20594));
      break;
    case (CLOSED_DOOR&0xff):
      mprint(LS(IDS_MSG_20595));
      break;
    case (LAVA&0xff):
      mprint(LS(IDS_MSG_20596));
      break;
    case (HEDGE&0xff):
      mprint(LS(IDS_MSG_20597));
      break;
    case (WATER&0xff):
      mprint(LS(IDS_MSG_20598));
      break;
    case (FIRE&0xff):
      mprint(LS(IDS_MSG_20599));
      break;
    case (TRAP&0xff):
      mprint(LS(IDS_MSG_20600));
      break;
    case (STAIRS_UP&0xff):
      mprint(LS(IDS_MSG_20601));
      break;
    case (STAIRS_DOWN&0xff):
      mprint(LS(IDS_MSG_20602));
      break;
    case (FLOOR&0xff):
      mprint(LS(IDS_MSG_20603));
      break;
    case (PORTCULLIS&0xff):
      mprint(LS(IDS_MSG_20604));
      break;
    case (ABYSS&0xff):
      mprint(LS(IDS_MSG_20605));
      break;
    case (PLAYER&0xff):
      mprint(LS(IDS_MSG_20606));
      break;
    case (CORPSE&0xff):
      mprint(LS(IDS_MSG_20607));
      break;
    case (THING&0xff):
      mprint(LS(IDS_MSG_20608));
      break;
    case (SAFE&0xff):
      mprint(LS(IDS_MSG_20609)); 
      break;
    case (RUBBLE&0xff):
      mprint(LS(IDS_MSG_20610));
      break;
    case (STATUE&0xff):
      mprint(LS(IDS_MSG_20611));
      break;
    case (ALTAR&0xff):
      mprint(LS(IDS_MSG_20612));
      break;
    case (CASH&0xff):
      mprint(LS(IDS_MSG_20613));
      break;
    case (PILE&0xff):
      mprint(LS(IDS_MSG_20614));
      break;
    case (FOOD&0xff):
      mprint(LS(IDS_MSG_20615));
      break;
    case (WEAPON&0xff):
      mprint(LS(IDS_MSG_20616));
      break;
    case (MISSILEWEAPON&0xff):
      mprint(LS(IDS_MSG_20617));
      break;
    case (SCROLL&0xff):
      mprint(LS(IDS_MSG_20618));
      break;
    case (POTION&0xff):
      mprint(LS(IDS_MSG_20619));
      break;
    case (ARMOR&0xff):
      mprint(LS(IDS_MSG_20620));
      break;
    case (SHIELD&0xff):
      mprint(LS(IDS_MSG_20621));
      break;
    case (CLOAK&0xff):
      mprint(LS(IDS_MSG_20622));
      break;
    case (BOOTS&0xff):
      mprint(LS(IDS_MSG_20623));
      break;
    case (STICK&0xff):
      mprint(LS(IDS_MSG_20624));
      break;
    case (RING&0xff):
      mprint(LS(IDS_MSG_20625));
      break;
    case (ARTIFACT&0xff):
      mprint(LS(IDS_MSG_20626));
      break;
    case (CHAIR&0xff):
      mprint(LS(IDS_MSG_20627));
      break;
    case (WHIRLWIND&0xff):
      mprint(LS(IDS_MSG_20628));
      break;
    default:
      mprint(LS(IDS_MSG_20629));
      break;
    }
  }
}


void wizard()
{
  char *lname;
#if !defined(MSDOS) && !defined(_WIN32)
  struct passwd *dastuff;
#endif

  setgamestatus(SKIP_MONSTERS);
  if (gamestatusp(CHEATED)) mprint(LS(IDS_MSG_20630));
  else {
    clearmsg();
    mprint(LS(IDS_MSG_20631));
    if (ynq()=='y') {
       lname = getlogin();
#if !defined(MSDOS) && !defined(_WIN32)
       if (!lname || strlen(lname) == 0)
       {
	    dastuff = getpwuid(getuid());
	    lname = dastuff->pw_name;
       }
#endif
       if (strcmp(lname,WIZARD)==0) {
	 setgamestatus(CHEATED);
	 mprint(LS(IDS_MSG_20632));
      }
      else {
	mprint(LS(IDS_MSG_20633));
	strcpy(Str1,WIZARD);
	strcat(Str1,LS(IDS_MSG_23194));
	if (Str1[0] >= 'a' && Str1[0] <= 'z')
	    Str1[0] += 'A'-'a'; /* capitalise 1st letter */
	mprint(Str1);
	mprint(LS(IDS_MSG_20634));
	if (random_range(2)) mprint(LS(IDS_MSG_20635));
	else mprint(LS(IDS_MSG_20636));
      }
    }
  }
}

/* Jump, that is */
void vault()
{
  int x=Player.x,y=Player.y,jumper=0;
  
  clearmsg();

  if (Player.possessions[O_BOOTS] != NULL)
    if (Player.possessions[O_BOOTS]->usef == I_BOOTS_JUMPING)
      jumper = 2;
  if (Player.status[IMMOBILE] > 0) {
    resetgamestatus(FAST_MOVE);
    print3(LS(IDS_MSG_20514));
  }
  else {
    setgamestatus(SKIP_MONSTERS);
    mprint(LS(IDS_MSG_20637));
    setspot(&x,&y);
    if (! los_p(Player.x,Player.y,x,y))
      print3(LS(IDS_MSG_20638));
    else if (Player.itemweight > Player.maxweight) 
      print3(LS(IDS_MSG_20639));
    else if (distance(x,y,Player.x,Player.y) > 
	     max(2,statmod(Player.agi)+2)+jumper)
      print3(LS(IDS_MSG_20640));
    else if (Level->site[x][y].creature != NULL)
      print3(LS(IDS_MSG_20641));
    else if (! p_moveable(x,y))
      print3(LS(IDS_MSG_20642));
    else {
      resetgamestatus(SKIP_MONSTERS);
      Player.x = x;
      Player.y = y;
      if ((! jumper) && (random_range(30) > Player.agi)) {
	mprint(LS(IDS_MSG_20643));
	setgamestatus(SKIP_PLAYER);
	p_damage((Player.itemweight/250),UNSTOPPABLE,"clumsiness");
      }
      p_movefunction(Level->site[Player.x][Player.y].p_locf);
      if (Current_Environment != E_COUNTRYSIDE) 
	if ((Level->site[Player.x][Player.y].things != NULL) &&
	    (optionp(PICKUP)))
	  pickup();
    }
  }
}


/* Sets sequence of combat maneuvers. */
void tacoptions()
{
  int actionsleft,done,place;
  char defatt, *attstr, *defstr; /* for the default setting */
  int draw_again = 1;

  setgamestatus(SKIP_MONSTERS);

  done = FALSE;
  actionsleft = maneuvers();
  place = 0;
  do {
    if (draw_again) {
      menuclear();
      menuprint("Enter a combat maneuvers sequence.\n");
      menuprint("? for help, ! for default, backspace to start again,\n");
      menuprint(" RETURN to save sequence\n");
      showmenu();
      draw_again = 0;
    }
    clearmsg();
    mprint(LS(IDS_MSG_20644));
    mnumprint(actionsleft);
    switch(mgetc()) {
    case '?':
      combat_help();
      draw_again = 1;
      break;
    case 'a': case 'A':
      if (actionsleft < 1) print3(LS(IDS_MSG_20645));
      else {
	if (Player.possessions[O_WEAPON_HAND] == NULL) {
	  Player.meleestr[place] = 'C';
	  menuprint("\nPunch:");
	}
	else if (Player.possessions[O_WEAPON_HAND]->type == THRUSTING) {
	  Player.meleestr[place] = 'T';
	  menuprint("\nThrust:");
	}
	else if (Player.possessions[O_WEAPON_HAND]->type == STRIKING) {
	  Player.meleestr[place] = 'C';
	  menuprint("\nStrike:");
	}
	else {
	  menuprint("\nCut:");
	  Player.meleestr[place] = 'C';
	}
	place++;
	Player.meleestr[place]=getlocation();
	place++;
	actionsleft--;
      }
      break;
    case 'b': case 'B':
      if (actionsleft<1) print3(LS(IDS_MSG_20645));
      else {
	Player.meleestr[place] = 'B';
	if (Player.possessions[O_WEAPON_HAND] == NULL)
	  menuprint("\nDodge (from):");
	else if (Player.possessions[O_WEAPON_HAND]->type == THRUSTING) 
	  menuprint("\nParry:");
	else menuprint("\nBlock:");
	place++;
	Player.meleestr[place]= getlocation();
	place++;
	actionsleft--;
      }
      break;
    case 'l': case 'L':
      if (actionsleft<2) print3(LS(IDS_MSG_20646));
      else {
	if (Player.possessions[O_WEAPON_HAND] != NULL) {
	  if (Player.possessions[O_WEAPON_HAND]->type != MISSILE) {
	    menuprint("\nLunge:");
	    Player.meleestr[place] = 'L';
	    place++;
	    Player.meleestr[place]=getlocation();
	    place++;
	    actionsleft -= 2;
	  }
	  else {
	    print3(LS(IDS_MSG_20647));
	    morewait();
	  }
	}
	else {
	  print3(LS(IDS_MSG_20648));
	  morewait();
	}
      }
      break;
    case 'r': case 'R':
      if (actionsleft<2) print3(LS(IDS_MSG_20649));
      else {
	if (Player.possessions[O_WEAPON_HAND] != NULL) {
	  if (Player.possessions[O_WEAPON_HAND]->type == THRUSTING) {
	    Player.meleestr[place++] = 'R';
	    menuprint("\nRiposte:");
	    Player.meleestr[place++]=getlocation();
	    actionsleft -= 2;
	  }
	  else {
	    print3(LS(IDS_MSG_20650));
	    morewait();
	  }
	}
	else {
	  print3(LS(IDS_MSG_20650));
	  morewait();
	}
      }
      break;
    case BACKSPACE:
    case DELETE:
      place = 0;
      actionsleft=maneuvers();
      draw_again = 1;
      break;
    case '!':
      if (Player.possessions[O_WEAPON_HAND] == NULL) {
	defatt = 'C';
	attstr = "Punch";
      }
      else if (Player.possessions[O_WEAPON_HAND]->type == THRUSTING) {
	defatt = 'T';
	attstr = "Thrust";
      }
      else if (Player.possessions[O_WEAPON_HAND]->type == STRIKING) {
	defatt = 'C';
	attstr = "Strike";
      }
      else {
	defatt = 'C';
	attstr = "Cut";
      }
      if (Player.possessions[O_WEAPON_HAND] == NULL)
	defstr = "Dodge";
      else if (Player.possessions[O_WEAPON_HAND]->type == THRUSTING) 
	defstr = "Parry";
      else defstr = "Block";
      menuclear();
      menuprint("Enter a combat maneuvers sequence.\n");
      menuprint("? for help, ! for default, backspace to start again,\n");
      menuprint(" RETURN to save sequence\n\n");
      for(place=0;place<maneuvers();place++)
	if (place&1) { /* every 2nd time around */
	    Player.meleestr[place*2] = 'B';
	    Player.meleestr[(place*2)+1] = 'C';
	    menuprint(defstr);
	    menuprint(" Center.\n");
	}
	else {
	    Player.meleestr[place*2] = defatt;
	    Player.meleestr[(place*2)+1] = 'C';
	    menuprint(attstr);
	    menuprint(" Center.\n");
	}
      actionsleft = 0;
      showmenu();
      Player.meleestr[place*2]='\0';
      break;
    case RETURN:
    case LINEFEED:
    case ESCAPE:
      done = TRUE;
      break;
    }
    if (actionsleft < 1) morewait();
  } while (! done);
  xredraw();
  Player.meleestr[place] = 0;
}




/* Do the Artful Dodger trick */
void pickpocket()
{
  int dx,dy,index=0;
  struct monster *m;

  clearmsg();

  mprint(LS(IDS_MSG_20651));

  index = getdir();

  if (index == ABORT)
    setgamestatus(SKIP_MONSTERS);
  else {
    dx = Dirs[0][index];
    dy = Dirs[1][index];
    
    if ((! inbounds(Player.x+dx, Player.y+dy)) ||
	(Level->site[Player.x+dx][Player.y+dy].creature == NULL)) {
      print3(LS(IDS_MSG_20652));
      setgamestatus(SKIP_MONSTERS);
    }
    else {
      m = Level->site[Player.x+dx][Player.y+dy].creature;
      if (m->id == ML0+3) {
	mprint(LS(IDS_MSG_20653));
	mprint(LS(IDS_MSG_20654));
	if (Player.cash > 0) {
	  mprint(LS(IDS_MSG_20655));
	  Player.cash = 0;
	  dataprint();
	}
	else {
	  mprint(LS(IDS_MSG_20656));
	  morewait();
	  send_to_jail();
	}
      }
      else if (m->possessions == NULL) {
	mprint(LS(IDS_MSG_20657));
	mprint(LS(IDS_MSG_20658));
	m_status_set(m,HOSTILE);
      }
      else if (Player.dex*5+Player.rank[THIEVES]*20+random_range(100) >
	       random_range(100)+m->level*20) {
	mprint(LS(IDS_MSG_20659));
	mprint(LS(IDS_MSG_20660));
	mprint(itemid(m->possessions->thing));
	Player.alignment--;
	gain_experience(m->level*m->level);
	gain_item(m->possessions->thing);
	m->possessions = m->possessions->next;
      }
    }
  }
}



void rename_player()
{
  setgamestatus(SKIP_MONSTERS);
  clearmsg();
  mprint(LS(IDS_MSG_20661));
  strcpy(Str1,msgscanstring());
  if (strlen(Str1) == 0)
    mprint(Player.name);
  else {
    if (Str1[0] >= 'a' && Str1[0] <= 'z')
      Str1[0] += 'A' - 'a';
    strcpy(Player.name, Str1);
  }
  sprintf(Str1, "Henceforth, you shall be known as %s", Player.name);
  print2(Str1);
}


void abortshadowform()
{
  setgamestatus(SKIP_MONSTERS);
  if (Player.status[SHADOWFORM] && (Player.status[SHADOWFORM]<1000)) {
    mprint(LS(IDS_MSG_20662));
    Player.immunity[NORMAL_DAMAGE]--;
    Player.immunity[ACID]--;
    Player.immunity[THEFT]--;
    Player.immunity[INFECTION]--;
    mprint(LS(IDS_MSG_20102));
    Player.status[SHADOWFORM] = 0;
  }
}

void tunnel()
{
  int dir,ox,oy,aux;

  clearmsg();
  mprint(LS(IDS_MSG_20663));
  dir = getdir();
  if (dir == ABORT)
    setgamestatus(SKIP_MONSTERS);
  else {
    ox = Player.x + Dirs[0][dir];
    oy = Player.y + Dirs[1][dir];
    if (loc_statusp(ox,oy,SECRET))
      mprint(LS(IDS_MSG_20664));
    else if (Level->site[ox][oy].locchar != WALL) {
      print3(LS(IDS_MSG_20665));
      setgamestatus(SKIP_MONSTERS);
    }
    else {
      aux = Level->site[ox][oy].aux;
      if (random_range(20)==1){
	if (Player.possessions[O_WEAPON_HAND] == NULL) {
	  mprint(LS(IDS_MSG_20666));
	  p_damage(Player.str / 6,UNSTOPPABLE,"a broken fingernail");
	}
	else if ((Player.possessions[O_WEAPON_HAND]->type == THRUSTING) ||
		 ((Player.possessions[O_WEAPON_HAND]->type != STRIKING) &&
		  (Player.possessions[O_WEAPON_HAND]->fragility < 
		   random_range(20)))) {
	  mprint(LS(IDS_MSG_20667));
	  (void) damage_item(Player.possessions[O_WEAPON_HAND]);
	}
	else mprint(LS(IDS_MSG_20668));
      }
      if (Player.possessions[O_WEAPON_HAND] == NULL) {
	if ((aux > 0) && ((Player.str/3)+random_range(100) > aux)) {
	  mprint(LS(IDS_MSG_20669));
	  tunnelcheck();
	  Level->site[ox][oy].locchar = RUBBLE;
	  Level->site[ox][oy].p_locf = L_RUBBLE;
	  lset(ox, oy, CHANGED);
	}
	else mprint(LS(IDS_MSG_20670));
      }
      else if (Player.possessions[O_WEAPON_HAND]->type == THRUSTING) {
	if ((aux > 0) && 
	    (Player.possessions[O_WEAPON_HAND]->dmg*2+random_range(100) > 
	     aux)) {
	  mprint(LS(IDS_MSG_20669));
	  tunnelcheck();
	  Level->site[ox][oy].locchar = RUBBLE;
	  Level->site[ox][oy].p_locf = L_RUBBLE;
	  lset(ox, oy, CHANGED);
	}
	else mprint(LS(IDS_MSG_20671));
      }
      else if ((aux > 0) && 
	       (Player.possessions[O_WEAPON_HAND]->dmg+random_range(100) 
		> aux)) {
	mprint(LS(IDS_MSG_20669));
	tunnelcheck();
	Level->site[ox][oy].locchar = RUBBLE;
	Level->site[ox][oy].p_locf = L_RUBBLE;
	lset(ox, oy, CHANGED);
      }
      else mprint(LS(IDS_MSG_20664));
    }
  }
}




void hunt(terrain)
short terrain;     
{
  int fertility=0;
  switch(terrain) {
  case SWAMP: 
    mprint(LS(IDS_MSG_20672));
    break;
  case VOLCANO:
  case CASTLE:
  case TEMPLE:
  case CAVES:
  case STARPEAK:
  case MAGIC_ISLE:
  case DRAGONLAIR:
    mprint(LS(IDS_MSG_20673));
    break;
  case VILLAGE:
  case CITY:
    mprint(LS(IDS_MSG_20674));
    break;
  case ROAD:
    mprint(LS(IDS_MSG_20675));
    break;
  case CHAOS_SEA:
    mprint(LS(IDS_MSG_20676));
    break;
  case DESERT:
    mprint(LS(IDS_MSG_20677));
    Time += 100;
    hourly_check();  
    fertility = 10;
    break;
  case JUNGLE:
    mprint(LS(IDS_MSG_20678));
    Time += 100;
    hourly_check();
    fertility = 80;
    break;
  case PLAINS:
    mprint(LS(IDS_MSG_20679));
    Time += 100;
    hourly_check();
    fertility = 50;
    break;
  case TUNDRA:
    mprint(LS(IDS_MSG_20680));
    Time += 100;
    hourly_check();
    fertility = 30;
    break;
  case FOREST:
    mprint(LS(IDS_MSG_20681));
    Time += 100;
    hourly_check();
    fertility = 70;
    break;
  case MOUNTAINS: case PASS:
    mprint(LS(IDS_MSG_20682));
    Time += 100;
    hourly_check();  
    fertility = 30;
    break;
  case RIVER:
    mprint(LS(IDS_MSG_20683));
    Time += 100;
    hourly_check();  
    fertility = 80;
    break;
  }
  if (((Date % 360 < 60) || (Date % 360 > 300)) && 
      (terrain != DESERT) &&
      (terrain != JUNGLE)) {
    mprint(LS(IDS_MSG_20684));
    fertility = fertility / 2;
  }
  if (fertility > random_range(100)) {
    mprint(LS(IDS_MSG_20685));
    change_environment(E_TACTICAL_MAP);
  }
  else mprint(LS(IDS_MSG_20686));
}

void dismount_steed()
{
  pml ml;
  if (! gamestatusp(MOUNTED))
    print3(LS(IDS_MSG_20687));
  else if (Current_Environment == E_COUNTRYSIDE) {
    mprint(LS(IDS_MSG_20688));
    mprint(LS(IDS_MSG_20689));
    if (ynq()=='y') resetgamestatus(MOUNTED);
  }
  else {
    resetgamestatus(MOUNTED);;
    ml = ((pml) checkmalloc(sizeof(mltype)));
    ml->m = ((pmt) checkmalloc(sizeof(montype)));
    *(ml->m) = Monsters[HORSE];
    ml->m->x = Player.x;
    ml->m->y = Player.y;
    ml->m->status = MOBILE+SWIMMING;
    ml->next = Level->mlist;
    Level->site[Player.x][Player.y].creature = ml->m;
    Level->mlist = ml;
  }
  calc_melee();
}


void city_move()
{
  int site,x=Player.x,y=Player.y,toggle=FALSE;
  clearmsg();
  if (Current_Environment != E_CITY) {
    print3(LS(IDS_MSG_20690));
    setgamestatus(SKIP_MONSTERS);
  }
  else if (Player.status[IMMOBILE] > 0) 
    print3(LS(IDS_MSG_20691));
  else if (hostilemonstersnear()) {
    setgamestatus(SKIP_MONSTERS);
    print3(LS(IDS_MSG_20692));
  }
  else if (Level->site[Player.x][Player.y].aux == NOCITYMOVE)
    print3(LS(IDS_MSG_20693));
  else {
    print1(LS(IDS_MSG_20694));
    site = parsecitysite();
    if (site != ABORT) {
      mprint(LS(IDS_MSG_20695));
      morewait();
      while ((x != CitySiteList[site][1]) || (y != CitySiteList[site][2])) {
	toggle = ! toggle;
	if (toggle) {
	  Time++;
	  if (Time % 10 == 0) tenminute_check();
	  else minute_status_check();
	}
	x += sign(CitySiteList[site][1] - x);
	y += sign(CitySiteList[site][2] - y);
	screencheck(y);
	omshowcursor(x,y);
      }
      Player.x = x;
      Player.y = y;
      screencheck(Player.y);
      mprint(LS(IDS_MSG_20696));
      drawvision(Player.x, Player.y);
      morewait();
      p_movefunction(Level->site[x][y].p_locf);
    }
  }
}



void frobgamestatus()
{
  char response;
  long num;
  mprint(LS(IDS_MSG_20697));
  do response = (char) mcigetc();
  while ((response != 'r') && (response != 's') && (response != ESCAPE));
  if (response != ESCAPE) {
    mprint(LS(IDS_MSG_20698));
    num = (int) parsenum();
    if (num > -1) {
      num = pow2(num);
      if (num == CHEATED) {
	mprint(LS(IDS_MSG_20699));
	return;
      }
      if (response == 's') setgamestatus(num);
      else resetgamestatus(num);
      mprint(LS(IDS_MSG_20700));
    }
  }
}
      
