/* omega copyright (C) by Laurence Raphael Brothers, 1987,1988,1989 */
/* aux3.c */
/* some functions called by com.c, also see aux1.c, aux2.c */ 
/* This is a real grab bag file. It contains functions used by
   aux1.c and omega.c, as well as elsewhere. It is mainly here so aux1.c
   and aux2.c are not huge */
  
#include "glob.h"

/* check every ten minutes */
void tenminute_check()
{
  if (Time % 60 == 0) hourly_check();
  else {
    if (Current_Environment == Current_Dungeon)  wandercheck();
    minute_status_check();
    tenminute_status_check();
    if ((Player.status[DISEASED] < 1) && (Player.hp < Player.maxhp))
      Player.hp = min(Player.maxhp,Player.hp+Player.level+1);
    if (Current_Environment != E_COUNTRYSIDE && Current_Environment != E_ABYSS)
      indoors_random_event();
  }
}



/* hourly check is same as ten_minutely check except food is also
   checked, and since time moves in hours out of doors, also
   outdoors_random_event is possible */

void hourly_check()
{
  Player.food--;
  foodcheck();
  if (hour()==0) { /* midnight, a new day */
    moon_check();
    Date++;
  }
  torch_check();
  if (Current_Environment == Current_Dungeon)  wandercheck();
  minute_status_check();
  tenminute_status_check();
  if ((Player.status[DISEASED] == 0) && (Player.hp < Player.maxhp))
    Player.hp = min(Player.maxhp,Player.hp+Player.level+1);
  if (Current_Environment != E_COUNTRYSIDE && Current_Environment != E_ABYSS)
    indoors_random_event();
}




void indoors_random_event()
{
  pml ml;
  pol ol;
  switch(random_range(1000)) {
    case 0: 
      print3(LS(IDS_MSG_20160));
      morewait();
      break;
    case 1:
      print3(LS(IDS_MSG_20161));
      morewait();
      break;
    case 2:
      print3(LS(IDS_MSG_20162));
      morewait();
      break;
    case 3:
      print3(LS(IDS_MSG_20163));
      morewait();
      break;
    case 4:
      print3(LS(IDS_MSG_20164));
      morewait();
      for (ml=Level->mlist;ml!=NULL;ml=ml->next)
	if (ml->m->hp > 0) ml->m->hp = Monsters[ml->m->id].hp;
      Player.hp = max(Player.hp,Player.maxhp);
      break;
    case 5:
      print3(LS(IDS_MSG_20165));
      morewait();
      break;
    case 6:
      print3(LS(IDS_MSG_20166));
      p_damage(10,UNSTOPPABLE,"a cosmic ray");
      morewait();
      break;
    case 7:
      print3(LS(IDS_MSG_20167));
      Player.hp = ++Player.maxhp;
      Player.mana = max(Player.mana, calcmana());
      morewait();
      break;
    case 8:
      print3(LS(IDS_MSG_20168));
      morewait();
      Player.cash += Player.level*Player.level+1;
      break;
    case 9:
      print3(LS(IDS_MSG_20169));
      morewait();
      break;
    case 10:
      print3(LS(IDS_MSG_20170));
      morewait();
      ol = ((pol) checkmalloc(sizeof(oltype)));
      do ol->thing = create_object(difficulty());
      while (!ol->thing);
      ol->next = Level->site[Player.x][Player.y].things;
      Level->site[Player.x][Player.y].things = ol;
      pickup();
      break;
    case 11: 
      print3(LS(IDS_MSG_20171));
      morewait();
      hint();
      morewait();
      break;
    case 12:
      if (Balance > 0) {
	print3(LS(IDS_MSG_20172));
	Balance = 0;
      }
      else print3(LS(IDS_MSG_20173));
      break;
    case 13:
      if (Balance > 0) {
	print3(LS(IDS_MSG_20174));
	Balance += 5000;
      }
      else print3(LS(IDS_MSG_20175));
      break;
    }
  dataprint();
  showflags();
}



void outdoors_random_event()
{
  int num,i,j;
  pob ob;

  switch(random_range(300)) {
  case 0:
    switch(Country[Player.x][Player.y].current_terrain_type) {
    case TUNDRA: 
      mprint(LS(IDS_MSG_20176));
      break;
    case DESERT:
      mprint(LS(IDS_MSG_20177));
      break;
    default:
      if ((Date > 75) && (Date < 330))
	mprint(LS(IDS_MSG_20178));
      else mprint(LS(IDS_MSG_20176));
    }
    morewait();
    mprint(LS(IDS_MSG_20179));
    morewait();
    Precipitation+=random_range(12)+1;
    setgamestatus(LOST);
    break;
  case 1:
    mprint(LS(IDS_MSG_20180));
    mprint(LS(IDS_MSG_20181));
    morewait();
    mprint(LS(IDS_MSG_20182));
    morewait();
    mprint(LS(IDS_MSG_20182));
    morewait();
    print3(LS(IDS_MSG_20183));
    setgamestatus(LOST);
    break;
  case 2:
    mprint(LS(IDS_MSG_20184));
    morewait();
    mprint(LS(IDS_MSG_20185));
    morewait();
    ob = ((pob) checkmalloc(sizeof(objtype)));
    *ob = Objects[FOODID+1];
    gain_item(ob);
    break;
  case 3:
    if (Precipitation > 0) {
      mprint(LS(IDS_MSG_20186));
      p_damage(random_range(25),ELECTRICITY,"a lightning strike");
      morewait();
    }
    else mprint(LS(IDS_MSG_20187));
    break;
  case 4:
    mprint(LS(IDS_MSG_20188));
    morewait();
    l_commandant();
    break;
  case 5:
    mprint(LS(IDS_MSG_20189));
    morewait();
    mprint(LS(IDS_MSG_20190));
    morewait();
    num = random_range(300);
    if (num <10) {
      mprint(LS(IDS_MSG_20191));
      p_damage(random_range(100),UNSTOPPABLE,"a chaos storm");
      morewait();
    }
    else if (num < 20) {
      mprint(LS(IDS_MSG_20192));
      morewait();
      mprint(LS(IDS_MSG_20193));
      morewait();
      mprint(LS(IDS_MSG_20194));
      morewait();
      Player.str = Player.maxstr = Player.con = Player.maxcon =
	Player.dex = Player.maxdex = Player.agi = Player.maxagi =
	  Player.iq = Player.maxiq = Player.pow = Player.maxpow =
	    ((Player.maxstr+Player.maxcon+Player.maxdex+Player.maxagi+
	      Player.maxiq+Player.maxpow+12)/6);
    }
    else if (num < 30) {
      mprint(LS(IDS_MSG_20195));
      morewait();
      for(i=1;i<MAXITEMS;i++)
	if (Player.possessions[i] != NULL) {
	  Player.possessions[i]->plus++;
	  if (Player.possessions[i]->objchar == STICK)
	    Player.possessions[i]->charge+=10;
	  Player.possessions[i]->blessing+=10;
	}
      cleanse(1);
      mprint(LS(IDS_MSG_20196));
      morewait();
      Player.maxpow += 5;
      Player.pow += 5;
      Player.mana = Player.maxmana = calcmana() * 5;
      mprint(LS(IDS_MSG_20197));
      morewait();
      Player.con -= 5;
      Player.maxcon -= 5;
      if (Player.con < 3)
	p_death("congestive heart failure");
    }
    else if (num < 40) {
      mprint(LS(IDS_MSG_20198));
      morewait();
      dispel(-1);
      dispel(-1);
      Player.pow-=10;
      Player.mana=0;
    }
    else if (num < 60) {
      mprint(LS(IDS_MSG_20199));
      morewait();
      Player.x = random_range(WIDTH);
      Player.y = random_range(LENGTH);
      screencheck(Player.y);
    }
    else if (num < 70) {
      mprint(LS(IDS_MSG_20200));
      morewait();
      ob = ((pob) checkmalloc(sizeof(objtype)));
      make_artifact(ob,-1);
      gain_item(ob);
    }
    else if (num < 80) {
      if (gamestatusp(MOUNTED)) {
	mprint(LS(IDS_MSG_20201));
	morewait();
	mprint(LS(IDS_MSG_20202));
	morewait();
	mprint(LS(IDS_MSG_20203));
	morewait();
	resetgamestatus(MOUNTED);
      }
      else {
	mprint(LS(IDS_MSG_20204));
	morewait();
	mprint(LS(IDS_MSG_20205));
	morewait();
	setgamestatus(MOUNTED);
      }
    }
    else if (num < 90) {
      mprint(LS(IDS_MSG_20206));
      morewait();
      wish(1);
    }
    else if (num < 100) {
      mprint(LS(IDS_MSG_20207));
      morewait();
      mprint(LS(IDS_MSG_20208));
      morewait();
      mprint(LS(IDS_MSG_20209));
      morewait();
      Player.xp = 0;
      Player.level = 0;
      for (i=0;i<NUMRANKS;i++) Player.rank[i] = 0;
      for (i=0;i<NUMSPELLS;i++) Spells[i].known = FALSE;
      rename_player();
    }
    else {
      mprint(LS(IDS_MSG_20210));
      morewait();
      mprint(LS(IDS_MSG_20211));
      morewait();
      gain_experience(1000);
    }
    break;
  case 6: case 7: case 8: case 9: case 10:
    mprint(LS(IDS_MSG_20212));
    morewait();
    change_environment(E_TACTICAL_MAP);
    break;
  case 11:
    mprint(LS(IDS_MSG_20213));
    morewait();
    if (gamestatusp(LOST)) {
      resetgamestatus(LOST);
      mprint(LS(IDS_MSG_20214));
    }
    for(i=Player.x-5;i<Player.x+6;i++)
      for(j=Player.y-5;j<Player.y+6;j++)
	if (inbounds(i,j)) {
	  c_set(i, j, SEEN);
	  if (Country[i][j].current_terrain_type !=
	    Country[i][j].base_terrain_type) {
	    c_set(i, j, CHANGED);
	    Country[i][j].current_terrain_type = 
	      Country[i][j].base_terrain_type;
	  }
	}
    show_screen();
    break;
  case 12:
    if (! gamestatusp(MOUNTED)) {
      mprint(LS(IDS_MSG_20215));
      p_damage(1,UNSTOPPABLE,"blisters");
    }
    break;
  case 13:
    mprint(LS(IDS_MSG_20165));
    break;
  case 14:
    mprint(LS(IDS_MSG_20166));
    morewait();
    p_damage(10,UNSTOPPABLE,"a cosmic ray");
    break;
  case 15:
    mprint(LS(IDS_MSG_20169));
    break;
  case 16:
    mprint(LS(IDS_MSG_20216));
    morewait();
    hint();
    morewait();
    break;
  }
  dataprint();
  showflags();
}


char getlocation()
{
  char c = '\0';

  menuprint(" (enter location [HCL]) ");
  showmenu();
  while (c == '\0')
    switch (c = ((char) mcigetc())) {
      case 'h':menuprint(" High."); break;
      case 'c':menuprint(" Center."); break;
      case 'l':menuprint(" Low."); break;
      default: c = '\0'; break;
    }
  showmenu();
  return (c - 'a' + 'A');
}






/* chance for player to resist magic somehow */
/* hostile_magic ranges in power from 0 (weak) to 10 (strong) */
int magic_resist(hostile_magic)
int hostile_magic;
{
  if ((Player.rank[COLLEGE]+Player.rank[CIRCLE] > 0) &&
      (Player.level/2 + random_range(20) >
       hostile_magic + random_range(20))) {
    if (Player.mana > hostile_magic * hostile_magic) {
      mprint(LS(IDS_MSG_20217));
      Player.mana -= hostile_magic * hostile_magic;
      dataprint();
      return(TRUE);
    }
  }
  if (Player.level/4 + Player.status[PROTECTION] + random_range(20) > 
      hostile_magic + random_range(30)) {
    mprint(LS(IDS_MSG_20218));
    return(TRUE);
  }
  else return(FALSE);
}





void terrain_check(takestime)
int takestime;
{
  int faster = 0;

  if (Player.patron == DRUID) {
    faster = 1;
    switch(random_range(32)) {
      case 0:print2(LS(IDS_MSG_20219)); break;
      case 1:print2(LS(IDS_MSG_20220)); break;
    }
  }
  else if (gamestatusp(MOUNTED)) {
    faster = 1;
    switch(random_range(32)) {
      case 0:
      case 1:print2(LS(IDS_MSG_20221));break;
      case 2:print2(LS(IDS_MSG_20222));break;
      case 3:print2(LS(IDS_MSG_20223));break;
    }
  }
  else if (Player.possessions[O_BOOTS] &&
      Player.possessions[O_BOOTS]->usef == I_BOOTS_7LEAGUE) {
    takestime = 0;
    switch(random_range(32)) {
      case 0:print2(LS(IDS_MSG_20224)); break;
      case 1:print2(LS(IDS_MSG_20225)); break;
      case 2:print2(LS(IDS_MSG_20226)); break;
      case 3:print2(LS(IDS_MSG_20227));
	break;
    }
  }
  else if (Player.status[SHADOWFORM]) {
    faster = 1;
    switch(random_range(32)) {
      case 0:print2(LS(IDS_MSG_20228)); break;
      case 1:print2(LS(IDS_MSG_20229)); break;
    }
  }
  else switch(random_range(32)) {
    case 0:print2(LS(IDS_MSG_20230)); break;
    case 1:print2(LS(IDS_MSG_20231)); break;
  }
  switch(Country[Player.x][Player.y].current_terrain_type) {
  case RIVER:
    if ((Player.y < 6) && (Player.x > 20)) locprint("Star Lake.");
    else if (Player.y < 41) {
      if (Player.x < 10) locprint("Aerie River.");
      else locprint("The Great Flood.");
    }
    else if (Player.x < 42) locprint("The Swamp Runs.");
    else locprint("River Greenshriek.");
    if (takestime) {
      Time += 60;
      hourly_check();
      Time += 60;
      hourly_check();
      Time += 60;
      hourly_check();
    }
    break;
  case ROAD:
    locprint("A well-maintained road.");
    if (takestime) {
      Time += 60;
      hourly_check();
    }
    break;
  case PLAINS:
    locprint("A rippling sea of grass.");
    if (takestime) {
      Time += 60;
      hourly_check();
      if (! faster) {
	Time += 60;
	hourly_check();
      }
    }
    break;
  case TUNDRA:
    locprint("The Great Northern Wastes.");
    if (takestime) {
      Time += 60;
      hourly_check();
      if (! faster) {
	Time += 60;
	hourly_check();
      }
    }
    break;
  case FOREST:
    if (Player.y < 10) locprint("The Deepwood.");
    else if (Player.y < 18) locprint("The Forest of Erelon.");
    else if (Player.y < 46) locprint("The Great Forest.");
    if (takestime) {
      Time += 60;
      hourly_check();
      if (Player.rank[PRIESTHOOD] == 0 || Player.patron != DRUID) {
	Time += 60;
	hourly_check();
	if (! faster) {
	  Time += 60;
	  hourly_check();
	}
      }
    }
    break;
  case JUNGLE:
    locprint("Greenshriek Jungle.");
    if (takestime) {
      Time += 60;
      hourly_check();
      Time += 60;
      hourly_check();
      if (! faster) {
	Time += 60;
	hourly_check();
	Time += 60;
	hourly_check();
      }
    }
    break;
  case DESERT:
    locprint("The Waste of Time.");
    if (takestime) {
      Time += 60;
      hourly_check();
      Time += 60;
      hourly_check();
      if (! faster) {
	Time += 60;
	hourly_check();
	Time += 60;
	hourly_check();
      }
    }
    break;
  case MOUNTAINS:
    if ((Player.y < 9) && (Player.x < 12))
      locprint("The Magic Mountains");
    else if ((Player.y < 9) && (Player.y > 2) && (Player.x < 40))
      locprint("The Peaks of the Fist.");
    else if (Player.x < 52)
      locprint("The Rift Mountains.");
    else locprint("Borderland Mountains.");
    if (takestime) {
      Time += 60;
      hourly_check();
      Time += 60;
      hourly_check();
      Time += 60;
      hourly_check();
      Time += 60;
      hourly_check();
      Time += 60;
      hourly_check();
      if (! faster) {
	Time += 60;
	hourly_check();
	Time += 60;
	hourly_check();
	Time += 60;
	hourly_check();
      }
    }
    break;
  case PASS:
    locprint("A hidden pass.");
    if (takestime) {
      Time += 60;
      hourly_check();
    }
    break;
  case CHAOS_SEA:
    locprint("The Sea of Chaos.");
    if (takestime) {
      Time += 60;
      hourly_check();
    }
    mprint(LS(IDS_MSG_20232));
    morewait();
    l_chaos();
    break;
  case SWAMP:
    locprint("The Loathly Swamp.");
    if (takestime) {
      Time += 60;
      hourly_check();
      Time += 60;
      hourly_check();
      Time += 60;
      hourly_check();
      Time += 60;
      hourly_check();
      Time += 60;
      hourly_check();
      Time += 60;
      hourly_check();
      if (! faster) {
	Time += 60;
	hourly_check();
	Time += 60;
	hourly_check();
      }
    }
    break;
  case CITY:
    if (gamestatusp(LOST)) {
      resetgamestatus(LOST); 
      mprint(LS(IDS_MSG_20233));
    }
    locprint("Outside Rampart, the city.");
    break;
  case VILLAGE:
    if (gamestatusp(LOST)) {
      resetgamestatus(LOST); 
      mprint(LS(IDS_MSG_20234));
    }
    locprint("Outside a small village.");
    break;
  case CAVES:
    locprint("A deserted hillside.");
    if (takestime) {
      Time += 60;
      hourly_check();
    }
    mprint(LS(IDS_MSG_20235));
    break;
  case CASTLE:
    locprint("Near a fortified castle.");
    if (takestime) {
      Time += 60;
      hourly_check();
    }
    mprint(LS(IDS_MSG_20236));
    break;
  case TEMPLE:
    switch(Country[Player.x][Player.y].aux) {
    case ODIN: locprint("A rough-hewn granite temple."); break;
    case SET: locprint("A black pyramidal temple made of sandstone."); break;
    case ATHENA: locprint("A classical marble-columned temple."); break;
    case HECATE: locprint("A temple of ebony adorned with ivory."); break;
    case DRUID: locprint("A temple formed of living trees."); break;
    case DESTINY: locprint("A temple of some mysterious blue crystal."); break;
    }
    if (takestime) {
      Time += 60;
      hourly_check();
    }
    mprint(LS(IDS_MSG_20237));
    break;
  case MAGIC_ISLE:
    locprint("A strange island in the midst of the Sea of Chaos.");
    if (takestime) {
      Time += 60;
      hourly_check();
    }
    mprint(LS(IDS_MSG_20238));
    break;
  case STARPEAK:
    locprint("Star Peak.");
    if (takestime) {
      Time += 60;
      hourly_check();
    }
    mprint(LS(IDS_MSG_20239));
    break;
  case DRAGONLAIR:
    locprint("A rocky chasm.");
    if (takestime) {
      Time += 60;
      hourly_check();
    }
    mprint(LS(IDS_MSG_20240));
    break;
  case VOLCANO:
    locprint("HellWell Volcano.");
    if (takestime) {
      Time += 60;
      hourly_check();
    }
    mprint(LS(IDS_MSG_20241));
    break;
  default:
    locprint("I haven't any idea where you are!!!");
    break;
   }
  outdoors_random_event();
}



void countrysearch()
{
  int x,y;
  Time+=60;
  hourly_check();
  for (x=Player.x-1;x<Player.x+2;x++)
    for (y=Player.y-1;y<Player.y+2;y++)
      if (inbounds(x,y)) {
	if (Country[x][y].current_terrain_type !=
	    Country[x][y].base_terrain_type) {
	  clearmsg();
	  mprint(LS(IDS_MSG_20242));
	  Country[x][y].current_terrain_type=Country[x][y].base_terrain_type;
	  c_set(x, y, CHANGED);
	  mprint(LS(IDS_MSG_20243));
	  mprint(countryid(Country[x][y].base_terrain_type));
	}
      }
}

char *countryid(terrain)
short terrain;
{
  switch(terrain&0xff) {
  case MOUNTAINS&0xff:
    strcpy(Str1,"Almost impassable mountains");
    break;
  case PLAINS&0xff:
    strcpy(Str1,"Seemingly endless plains");
    break;
  case TUNDRA&0xff:
    strcpy(Str1,"A frosty stretch of tundra");
    break;
  case ROAD&0xff:
    strcpy(Str1,"A paved highway");
    break;
  case PASS&0xff:
    strcpy(Str1,"A secret mountain pass");
    break;
  case RIVER&0xff:
    strcpy(Str1,"A rolling river");
    break;
  case CITY&0xff:
    strcpy(Str1,"The city of Rampart");
    break;
  case VILLAGE&0xff:
    strcpy(Str1,"A rural village");
    break;
  case FOREST&0xff:
    strcpy(Str1,"A verdant forest");
    break;
  case JUNGLE&0xff:
    strcpy(Str1,"A densely overgrown jungle");
    break;
  case SWAMP&0xff:
    strcpy(Str1,"A swampy fen");
    break;
  case VOLCANO&0xff:
    strcpy(Str1,"A huge active volcano");
    break;
  case CASTLE&0xff:
    strcpy(Str1,"An imposing castle");
    break;
  case STARPEAK&0xff:
    strcpy(Str1,"A mysterious mountain.");
    break;
  case DRAGONLAIR&0xff:
    strcpy(Str1,"A cavern filled with treasure.");
    break;
  case MAGIC_ISLE&0xff:
    strcpy(Str1,"An island emanating magic.");
    break;
  case CAVES&0xff:
    strcpy(Str1,"A hidden cave entrance");
    break;
  case TEMPLE&0xff:
    strcpy(Str1,"A neoclassical temple");
    break;
  case DESERT&0xff:
    strcpy(Str1,"A sere desert");
    break;
  case CHAOS_SEA&0xff:
    strcpy(Str1,"The Sea of Chaos");
    break;
  default:
    strcpy(Str1,"I have no idea.");
    break;
  }
  return(Str1);
}

static char *sitenames[] = {	/* alphabetical listing */
"alchemist", "arena", "armorer", "bank", "brothel", "casino", "castle",
"city gates", "collegium magii", "condo", "department of public works",
"diner", "explorers' club", "fast food", "gymnasium", "healer", "hospice",
"les crapuleux", "library", "mercenary guild", "oracle", "order of paladins",
"pawn shop", "sorcerors' guild ", "tavern", "temple", "thieves' guild" };

static int sitenums[] = {	/* the order matches sitenames[] */
 L_ALCHEMIST, L_ARENA, L_ARMORER, L_BANK, L_BROTHEL, L_CASINO, L_CASTLE,
 L_COUNTRYSIDE, L_COLLEGE, L_CONDO, L_DPW, L_DINER, L_CLUB, L_COMMANDANT,
 L_GYM, L_HEALER, L_CHARITY, L_CRAP, L_LIBRARY, L_MERC_GUILD, L_ORACLE,
 L_ORDER, L_PAWN_SHOP, L_SORCERORS, L_TAVERN, L_TEMPLE, L_THIEVES_GUILD };

void showknownsites(first, last)
int first, last;
{
  int i,printed=FALSE;

  menuclear();
  menuprint("\nPossible Sites:\n");
  for (i = first; i <= last; i++)
    if (CitySiteList[sitenums[i] - CITYSITEBASE][0]) {
      printed = TRUE;
      menuprint(sitenames[i]);
      menuprint("\n");
    }
  if (! printed)
    menuprint("\nNo known sites match that prefix!");
  showmenu();
}

int parsecitysite()
{
  int first, last, pos;
  char byte, prefix[80];
  int found = 0;
  int f, l;

  first = 0;
  last = NUMCITYSITES - 1;
  pos = 0;
  print2(LS(IDS_MSG_20244));
  do {
    byte = mgetc();
    if (byte == BACKSPACE || byte == DELETE) {
      if (pos > 0) {
        prefix[--pos] = '\0';
	byte = prefix[pos - 1];
	f = first;
	while (f >= 0 && !strncmp(prefix, sitenames[f], pos)) {
	  if (CitySiteList[sitenums[f] - CITYSITEBASE][0])
	    first = f;
	  f--;
	}
	l = last;
	while (l < NUMCITYSITES && !strncmp(prefix, sitenames[l], pos)) {
	  if (CitySiteList[sitenums[l] - CITYSITEBASE][0])
	    last = l;
	  l++;
	}
	if (found)
	  found = 0;
	print2(prefix);
      }
      if (pos == 0) {
	first = 0;
	last = NUMCITYSITES - 1;
	found = 0;
	print2(LS(IDS_MSG_20244));
      }
    }
    else if (byte == ESCAPE) {
      xredraw();
      return ABORT;
    }
    else if (byte == '?')
      showknownsites(first, last);
    else if (byte != '\n') {
      if (byte >= 'A' && byte <= 'Z') 
	byte += 'a' - 'A';
      if (found)
	continue;
      f = first;
      l = last;
      while (f < NUMCITYSITES &&
	  (!CitySiteList[sitenums[f] - CITYSITEBASE][0] ||
	  strlen(sitenames[f]) < pos || sitenames[f][pos] < byte))
	f++;
      while (l >= 0 && (!CitySiteList[sitenums[l] - CITYSITEBASE][0] ||
	  strlen(sitenames[l]) < pos || sitenames[l][pos] > byte))
	l--;
      if (l < f)
	continue;
      prefix[pos++] = byte;
      prefix[pos] = '\0';
      nprint2(prefix + pos - 1);
      first = f;
      last = l;
      if (first == last && !found) {	/* unique name */
	found = 1;
	nprint2(sitenames[first] + pos);
      }
    }
  } while (byte != '\n');
  xredraw();
  if (found)
    return sitenums[first] - CITYSITEBASE;
  else {
    print3(LS(IDS_MSG_20245));
    return ABORT;
  }
}


/* are there hostile monsters within 2 moves? */
int hostilemonstersnear()
{
  int i,j,hostile = FALSE;

  for (i=Player.x-2;((i<Player.x+3)&&(! hostile));i++)
    for (j=Player.y-2;((j<Player.y+3)&&(! hostile));j++)
      if (inbounds(i,j))
	if (Level->site[i][j].creature != NULL)
	  hostile = m_statusp(Level->site[i][j].creature,HOSTILE);
  
  return(hostile);
}

/* random effects from some of stones in villages */  
/* if alignment of stone is alignment of player, gets done sooner */
int stonecheck(alignment)
int alignment;
{
  int *stone,match=FALSE,cycle=FALSE,i;

  if (alignment == 1) {
    stone = &Lawstone;
    match = Player.alignment > 0;
  }
  else if (alignment == -1) {
    stone = &Chaostone;
    match = Player.alignment < 0;
  }
  else {
    stone = &Mindstone;
    match = FALSE;
  }
  *stone += random_range(4) + (match ? random_range(4) : 0);
  switch((*stone)++) {
  case 0:case 2:case 4:case 6:case 8:case 10:case 12:case 14:case 16:case 18:
  case 20:case 22:case 24:case 26:case 28:case 30:case 32:case 34:case 36:
  case 38:case 40: print1(LS(IDS_MSG_20246));
    print2(LS(IDS_MSG_20247)); 
    (*stone)--;
    break;
  case 1: print1(LS(IDS_MSG_20248)); 
    print2(LS(IDS_MSG_20249));
    print3(LS(IDS_MSG_20250));
    for(i=0;i<MAXPACK;i++) 
      if (Player.pack[i] != NULL) {
	free((char *) Player.pack[i]);
	Player.pack[i] = NULL;
      }
    Player.packptr = 0;
    break;
  case 3: print1(LS(IDS_MSG_20251));
    print2(LS(IDS_MSG_20252));
    morewait();
    dispel(-1);
    break;
  case 5: print1(LS(IDS_MSG_20253));
    print2(LS(IDS_MSG_20254));
    summon(-1,-1);
    morewait();
    break;
  case 7: print1(LS(IDS_MSG_20255));
    print2(LS(IDS_MSG_20256));
    Player.cash++;
    break;
  case 9: print1(LS(IDS_MSG_20257));
    print2(LS(IDS_MSG_20258));
    augment(1);
    break;
  case 11: print1(LS(IDS_MSG_20259));
    print2(LS(IDS_MSG_20260)); 
    manastorm(Player.x,Player.y,random_range(Player.maxhp)+1);
    break;
  case 13: print1(LS(IDS_MSG_20261));
    print2(LS(IDS_MSG_20262));
    Player.status[BERSERK] += 10;
    break;
  case 15: print1(LS(IDS_MSG_20263));
    print2(LS(IDS_MSG_20264));
    morewait();
    print2(LS(IDS_MSG_20265));
    break;
  case 17: print1(LS(IDS_MSG_20266));
    print2(LS(IDS_MSG_20267));
    Player.agi -= 3;
    break;
  case 19: print1(LS(IDS_MSG_20268));
    print2(LS(IDS_MSG_20269));
    Player.status[DISEASED] = 1100;
    Player.hp = 1;
    Player.dex -= 5;
    break;
  case 21: print1(LS(IDS_MSG_20270));
    print2(LS(IDS_MSG_20271));
    Player.status[HERO]+=10;
    break;
  case 23: print1(LS(IDS_MSG_20272));
    print2(LS(IDS_MSG_20273));
    morewait();
    strategic_teleport(-1);
    break;
  case 25: print1(LS(IDS_MSG_20274));
    morewait();
    print1(LS(IDS_MSG_20275));
    morewait();
    hint();
    break;
  case 27: print1(LS(IDS_MSG_20276));
    print2(LS(IDS_MSG_20277));
    morewait();
    clearmsg();
    identify(1);
    knowledge(1);
    break;
  case 29: print1(LS(IDS_MSG_20278));
    print2(LS(IDS_MSG_20279));
    for(i=0;i<NUMSPELLS;i++) 
      if (Spells[i].known) {
	Spells[i].known = FALSE;
	break;
      }
    break;
  case 31: print1(LS(IDS_MSG_20280));
    morewait();
    acquire(0);
    break;
  case 33: print1(LS(IDS_MSG_20281));
    print2(LS(IDS_MSG_20282));
    Player.status[INVISIBLE]+=10;
    break;
  case 35: print1(LS(IDS_MSG_20283));
    print2(LS(IDS_MSG_20284));
    p_damage(random_range(Player.maxhp),ELECTRICITY,"mystic lightning");
    break;
  case 37: print1(LS(IDS_MSG_20285));
    print2(LS(IDS_MSG_20286));
    gain_experience((Player.level+1)*250);
    break;
  case 39: print1(LS(IDS_MSG_20287)); 
    print2(LS(IDS_MSG_20288)); 
    morewait();
    enchant(1);
    bless(1);
    print3(LS(IDS_MSG_20289));
    break;
  case 41:case 42:case 43:case 44:case 45:case 46:case 47:case 48:case 49:
    print1(LS(IDS_MSG_20290));
    print2(LS(IDS_MSG_20291));
    *stone = 50;
    cycle = TRUE;
    break;  
  default: print1(LS(IDS_MSG_20292));
    print2(LS(IDS_MSG_20273));
    morewait();
    *stone = 0;
    strategic_teleport(-1);
    break;
  }
  calc_melee();
  return(cycle);
}

void alert_guards()
{
  int foundguard=FALSE;
  pml ml;
  int suppress = 0;
  for(ml=Level->mlist;ml!=NULL;ml=ml->next)
    if (((ml->m->id == ML0+3) ||  /*guard*/
	 ((ml->m->id == ML0+8) && (ml->m->aux2 == 15))) && /*justiciar*/
	(ml->m->hp > 0)) {
      foundguard=TRUE;
      m_status_set(ml->m,AWAKE);
      m_status_set(ml->m,HOSTILE);
    }
  if (foundguard) {
    mprint(LS(IDS_MSG_20293));
    if (Current_Environment == E_CITY)
      Level->site[40][60].p_locf = L_NO_OP; /* pacify_guards restores this */
  }
  if ((! foundguard) && (Current_Environment == E_CITY) &&
      !gamestatusp(DESTROYED_ORDER)) {
    suppress = gamestatusp(SUPPRESS_PRINTING);
    resetgamestatus(SUPPRESS_PRINTING);
    print2(LS(IDS_MSG_20294));
    morewait();
    gain_experience(1000);
    Player.alignment -= 250;
    if (! gamestatusp(KILLED_LAWBRINGER)) {
      print1(LS(IDS_MSG_20295));
      print2(LS(IDS_MSG_20296));
      print3(LS(IDS_MSG_20297));
      morewait();
      print1(LS(IDS_MSG_20298));
      print2(LS(IDS_MSG_20299));
      print3(LS(IDS_MSG_20300));
      morewait();
      Player.str /= 2;
      dataprint();
      print1(LS(IDS_MSG_20301));
      print2(LS(IDS_MSG_20302));
      print3(LS(IDS_MSG_20303));
      morewait();
      resurrect_guards();
    }
    else {
      print1(LS(IDS_MSG_20304));
      print2(LS(IDS_MSG_20305));
      morewait();
      print1(LS(IDS_MSG_20306));
      print2(LS(IDS_MSG_20307));
      print3(LS(IDS_MSG_20308));
      morewait();
      gain_experience(5000);
      destroy_order();
    }
  }
  if (suppress)
    resetgamestatus(SUPPRESS_PRINTING);
}


/* can only occur when player is in city, so OK to use Level */
void destroy_order()
{
  int i,j;
  setgamestatus(DESTROYED_ORDER);
  if (Level != City) print1(LS(IDS_MSG_20309));
  else 
    for(i=35;i<46;i++)
      for(j=60;j<63;j++) {
	if (i == 40 && (j == 60 || j == 61)) {
	  lreset(i,j,SECRET);
	  Level->site[i][j].locchar = FLOOR;
	  Level->site[i][j].p_locf = L_NO_OP;
	  lset(i, j, CHANGED);
	}
	else {
	  Level->site[i][j].locchar = RUBBLE;
	  Level->site[i][j].p_locf = L_RUBBLE;
	  lset(i, j, CHANGED);
	}
	if (Level->site[i][j].creature != NULL) {
	  Level->site[i][j].creature->hp = -1;
	  Level->site[i][j].creature = NULL;
	}
	make_site_monster(i,j,ML2+6);
	Level->site[i][j].creature->monstring = "ghost of a Paladin";
	m_status_set(Level->site[i][j].creature,HOSTILE);
      }
}



int maneuvers()
{
  int m;

  m = 2 + Player.level/7;
  if (Player.rank[ARENA]) m++;
  if (Player.status[HASTED])
    m *= 2;
  if (Player.status[SLOWED])
    m /= 2;
  m = min(8,max(1,m));
  
  return(m);
}

/* for when haste runs out, etc. */
void default_maneuvers()
{
  int i;
  morewait();
  clearmsg();
  print1(LS(IDS_MSG_20310));
  print2(LS(IDS_MSG_20311));
  morewait();
  for(i=0;i<maneuvers();i+=2) {
    Player.meleestr[i*2]='A';
    Player.meleestr[(i*2)+1]='C';
    Player.meleestr[(i+1)*2]='B';
    Player.meleestr[((i+1)*2)+1]='C';
  }
  Player.meleestr[maneuvers()*2]=0;
}
