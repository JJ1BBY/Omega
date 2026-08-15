/* omega copyright (C) by Laurence Raphael Brothers, 1987,1988,1989 */
/* move.c */
/* general functions for player moving */

#include "glob.h"



/* various miscellaneous location functions */
void l_water()
{
  if (! gamestatusp(MOUNTED))  {
    if ((Player.possessions[O_ARMOR] != NULL)) {
      print1(LS(IDS_MSG_21652));
      morewait();
      p_drown();
      print2(LS(IDS_MSG_21653));
    }
    else if (Player.itemweight > ((int) (Player.maxweight / 2))) {
      print1(LS(IDS_MSG_21654));
      morewait();
      p_drown();
      print2(LS(IDS_MSG_21653));
    }
    else switch(random_range(32)) {
    case 0:print1(LS(IDS_MSG_21655)); break;
    case 1:print1(LS(IDS_MSG_21656)); break;
    case 2:print1(LS(IDS_MSG_21657)); break;
    case 3:print1(LS(IDS_MSG_21658)); break;
    }
  }
  else switch(random_range(32)) {
    case 0:print1(LS(IDS_MSG_21659)); break;
    case 1:print1(LS(IDS_MSG_21660)); break;
    case 2:print1(LS(IDS_MSG_21661)); break;
    case 3:print1(LS(IDS_MSG_21662)); break;
    }
}



void l_chaos()
{
  if (gamestatusp(MOUNTED)) {
    print1(LS(IDS_MSG_21663));
    print2(LS(IDS_MSG_21664));
    morewait();
    print1(LS(IDS_MSG_21665));
    morewait();
    resetgamestatus(MOUNTED);
  }
  if (! onewithchaos)
    print1(LS(IDS_MSG_21666));
  if (Player.rank[ADEPT]) {
    if (! onewithchaos)
    {
      onewithchaos = 1;
      print2(LS(IDS_MSG_21667));
    }
    Player.mana = max(Player.mana,calcmana());
    Player.hp = max(Player.hp, Player.maxhp);
  }
  else if (Player.rank[PRIESTHOOD] && (! saved)) {
    print2(LS(IDS_MSG_21668));
    print3(LS(IDS_MSG_21669));
    gain_experience(500);
    saved = TRUE;
  }
  else {
    print2(LS(IDS_MSG_21670));
    if (saved) nprint2(LS(IDS_MSG_21671));
    morewait();
    print1(LS(IDS_MSG_21672));
    Player.alignment -= 50;
    gain_experience(1000);
    p_death("immersion in raw Chaos");
  }
}




void l_hedge()
{
  if (Player.patron == DRUID) print1(LS(IDS_MSG_21673));
  else {
    print1(LS(IDS_MSG_21674));
    switch(random_range(6)) {
    case 0: 
      print2(LS(IDS_MSG_21675));
      p_damage(random_range(6),NORMAL_DAMAGE,"a hedge");
      print3(LS(IDS_MSG_21676));
      p_poison(random_range(12));
      break;
    case 1: 
      print2(LS(IDS_MSG_21675));
      p_damage(random_range(12),NORMAL_DAMAGE,"a hedge");
      break;
    case 2: 
      print2(LS(IDS_MSG_21677));
      Player.status[IMMOBILE]+=random_range(5)+1;
      break;
    case 3: 
      if (Player.possessions[O_CLOAK] != NULL) {
	print2(LS(IDS_MSG_21678));
	dispose_lost_objects(1,Player.possessions[O_CLOAK]);
      }
      else print2(LS(IDS_MSG_21679));
      break;
    default: print2(LS(IDS_MSG_21680)); break;
    }
  }
}



void l_lava()
{
  print1(LS(IDS_MSG_21681));
  if (gamestatusp(MOUNTED)) {
    print2(LS(IDS_MSG_21682));
    resetgamestatus(MOUNTED);
  }
  morewait();
  if (strcmp(Player.name,"Saltheart Foamfollower")==0) {
    print1(LS(IDS_MSG_21683));
    p_damage(1,UNSTOPPABLE,"slow death in a pool of lava");
  }
  else {
    p_damage(random_range(75),FLAME,"incineration in a pool of lava");
    if (Player.hp> 0) p_drown();
    Player.status[IMMOBILE]+=2;
  }
}



void l_fire()
{
  print1(LS(IDS_MSG_21684));
  if (gamestatusp(MOUNTED)) {
    print2(LS(IDS_MSG_21685));
    resetgamestatus(MOUNTED);
  }
  p_damage(random_range(100),FLAME,"self-immolation");
}

void l_abyss()
{
  int i;
  if (Current_Environment != Current_Dungeon) {
    print1(LS(IDS_MSG_21686));
    morewait();
    strategic_teleport(-1);
  }
  else {
    print1(LS(IDS_MSG_21687));
    morewait();
    if (random_range(100)==13) {
      print1(LS(IDS_MSG_21688));
      print2(LS(IDS_MSG_21689));
      print3(LS(IDS_MSG_21690));
      morewait();
      clearmsg();
      if (Player.alignment > -10) 
	p_death("the Eater of Souls");
      else {
	print1(LS(IDS_MSG_21691));
	print2(LS(IDS_MSG_21692));
	morewait();
	change_level(Level->depth,Level->depth+1,FALSE);
	gain_experience(2000);
	Player.alignment -= 50;
      }
    }
    else {
      i = 0;
      print1(LS(IDS_MSG_21693));
      while(random_range(3)!=2) {
	if (i%6 == 0)
	    print2(LS(IDS_MSG_21694));
	else
	    nprint2(LS(IDS_MSG_21694));
	i++;
	morewait();
      }
      i++;
      print1(LS(IDS_MSG_21695));
      morewait();
      if (Level->depth+i>MaxDungeonLevels) {
	print2(LS(IDS_MSG_21696));
	print3(LS(IDS_MSG_21697));
	morewait();
	change_environment(E_COUNTRYSIDE);
	do {
	  Player.x = random_range(WIDTH);
	  Player.y = random_range(LENGTH);
	} while(Country[Player.x][Player.y].base_terrain_type == CHAOS_SEA);
	p_damage(i*50,NORMAL_DAMAGE,"a fall from a great height");
      }
      else {
	print2(LS(IDS_MSG_21698));
	morewait();
	p_damage(i*5,NORMAL_DAMAGE,"a fall through the abyss");
	change_level(Level->depth,Level->depth+i,FALSE);
	gain_experience(i*i*50);
      }
    }
  }
}


void l_lift()
{
  char response;
  int levelnum;
  int distance;
  int too_far = 0;

  Level->site[Player.x][Player.y].locchar = FLOOR;
  Level->site[Player.x][Player.y].p_locf = L_NO_OP;
  lset(Player.x, Player.y, CHANGED);
  print1(LS(IDS_MSG_21699));
  print2(LS(IDS_MSG_21700));
  print3(LS(IDS_MSG_21701));
  morewait();
  clearmsg();
  print1(LS(IDS_MSG_21702));
  do response = (char) mcigetc();
  while ((response != 'u') && 
	 (response != 'd') &&
	 (response != ESCAPE));
  if (response != ESCAPE) {
    print1(LS(IDS_MSG_21703));
    levelnum = (int) parsenum();
    if (levelnum > 6) {
      too_far = 1;
      levelnum = 6;
    }
    if (response == 'u' && Level->depth - levelnum < 1) {
      distance = levelnum - Level->depth;
      change_environment(E_COUNTRYSIDE); /* "you return to the countryside." */
      if (distance > 0) {
	nprint1(LS(IDS_MSG_21704));
	print2(LS(IDS_MSG_21705));
	morewait();
	print3(LS(IDS_MSG_21706));
	morewait();
	print3(LS(IDS_MSG_21697));
	p_damage(distance*10,NORMAL_DAMAGE,"a fall from a great height");
      }
      return;
    }
    else if (response == 'd' && Level->depth + levelnum > MaxDungeonLevels) {
      too_far = 1;
      levelnum = MaxDungeonLevels - Level->depth;
    }
    if (levelnum == 0) {
      print1(LS(IDS_MSG_20883));
      return;
    }
    if (too_far) {
      print1(LS(IDS_MSG_21707));
      print2(LS(IDS_MSG_21708));
    }
    else
      print1(LS(IDS_MSG_21708));
    change_level(Level->depth,
		(response=='d' ? 
		 Level->depth+levelnum : 
		 Level->depth-levelnum),
		FALSE);
    roomcheck();
  }
}


void l_magic_pool()
{
  int possibilities=random_range(100);
  print1(LS(IDS_MSG_21709));
  if (gamestatusp(MOUNTED)) {
    if (random_range(2)) {
      print2(LS(IDS_MSG_21710));
      resetgamestatus(MOUNTED);
    }
    else print2(LS(IDS_MSG_21711));
  }
  else  if (possibilities == 0) {
    print1(LS(IDS_MSG_21712));
    if (random_range(1000) < Player.level*Player.level*Player.level) {
      print2(LS(IDS_MSG_21713));
      print3(LS(IDS_MSG_21714));
      gain_experience(500);
      Player.hp = 1;
    }
    else p_death("the DREADED AQUAE MORTIS!");
  }
  else if (possibilities < 25)
    augment(0);
  else if (possibilities < 30)
    augment(1);
  else if (possibilities < 60)
    augment(-1);
  else if (possibilities < 65)
    cleanse(1);
  else if (possibilities < 80) {
    if (Player.possessions[O_WEAPON_HAND] != NULL) {
      print1(LS(IDS_MSG_21715));
      dispose_lost_objects(1,Player.possessions[O_WEAPON_HAND]);
    }
    else print1(LS(IDS_MSG_20085));
  }
  else if (possibilities < 90) {
    if (Player.possessions[O_WEAPON_HAND] != NULL) {
      print1(LS(IDS_MSG_21716));
      Player.possessions[O_WEAPON_HAND]->plus += random_range(10)+1;
      calc_melee();
    }
    else print1(LS(IDS_MSG_21717));
  }
  else if (possibilities < 95) {
    Player.hp += 10;
    print1(LS(IDS_MSG_21718));
  }
  else if (possibilities < 99) {
    print1(LS(IDS_MSG_21719));
    p_poison(10);
  }
  else if (possibilities == 99) {
    print1(LS(IDS_MSG_21720));
    heal(10);
    cleanse(1);
    Player.mana = calcmana()*3;
    Player.str = (Player.maxstr++)*3;
  }
  print2(LS(IDS_MSG_21721));
  Level->site[Player.x][Player.y].locchar = TRAP;
  Level->site[Player.x][Player.y].p_locf = L_TRAP_PIT;
  lset(Player.x, Player.y, CHANGED);
}
    












void l_no_op()
{
}


void l_tactical_exit()
{
  /* Free up monsters and items, and the level, if not SAVE_LEVELS */
  free_level(Level);
  Level = NULL;
  if ((Current_Environment == E_TEMPLE) || 
      (Current_Environment == E_TACTICAL_MAP) )
    change_environment(E_COUNTRYSIDE);
  else change_environment(Last_Environment);
}


void l_rubble()
{
  int screwup = random_range(100) - (Player.agi + Player.level); 
  print1(LS(IDS_MSG_21722));
  if (screwup < 0) print2(LS(IDS_MSG_21723));
  else {
    print2(LS(IDS_MSG_21724));
    print3(LS(IDS_MSG_21725));
    Player.status[IMMOBILE]+=2;
    p_damage(screwup/5,UNSTOPPABLE,"rubble and debris");
    morewait();
  }
}





/* Drops all portcullises in 5 moves */
void l_portcullis_trap()
{
  int i,j,slam=FALSE;
  
  print3(LS(IDS_MSG_21726));
  morewait();
  for (i=max(Player.x-5,0);i<min(Player.x+6,WIDTH);i++)
    for(j=max(Player.y-5,0);j<min(Player.y+6,LENGTH);j++) {
      if ((Level->site[i][j].p_locf == L_PORTCULLIS) &&
	  (Level->site[i][j].locchar != PORTCULLIS)) {
	Level->site[i][j].locchar = PORTCULLIS;
	lset(i, j, CHANGED);
	putspot(i,j,PORTCULLIS);
	if ((i==Player.x)&&(j==Player.y)) {
	  print3(LS(IDS_MSG_21727));
	  morewait();
	  p_damage(random_range(1000),NORMAL_DAMAGE,"a portcullis");
	}
	slam = TRUE;
      }
    }
  if (slam) print3(LS(IDS_MSG_21728));
}

/* drops every portcullis on level, then kills itself and all similar traps. */
void l_drop_every_portcullis()
{
  int i,j,slam=FALSE;
  
  print3(LS(IDS_MSG_21726));
  morewait();
  for (i=0;i<WIDTH;i++)
    for(j=0;j<LENGTH;j++) {
      if (Level->site[i][j].p_locf == L_DROP_EVERY_PORTCULLIS) {
	Level->site[i][j].p_locf = L_NO_OP;
	lset(i, j, CHANGED);
      }
      else if ((Level->site[i][j].p_locf == L_PORTCULLIS) &&
	  (Level->site[i][j].locchar != PORTCULLIS)) {
	Level->site[i][j].locchar = PORTCULLIS;
	lset(i, j, CHANGED);
	putspot(i,j,PORTCULLIS);
	if ((i==Player.x)&&(j==Player.y)) {
	  print3(LS(IDS_MSG_21727));
	  morewait();
	  p_damage(random_range(1000),NORMAL_DAMAGE,"a portcullis");
	}
	slam = TRUE;
      }
    }
  if (slam) print3(LS(IDS_MSG_21728));
}



void l_raise_portcullis()
{
  int i,j,open=FALSE;
  for (i=0;i<WIDTH;i++)
    for(j=0;j<LENGTH;j++) {
      if (Level->site[i][j].locchar == PORTCULLIS) {
	Level->site[i][j].locchar = FLOOR;
	lset(i, j, CHANGED);
	putspot(i,j,FLOOR);
	open = TRUE;
      }
    }
  if (open) print1(LS(IDS_MSG_21729));
}


void l_arena_exit()
{
  resetgamestatus(ARENA_MODE);
#ifndef MSDOS
  free_level(Level);
#endif
  Level = NULL;
  change_environment(E_CITY);
}


void l_house_exit()
{
#ifndef MSDOS
  free_level(Level);
#endif
  Level = NULL;
  change_environment(Last_Environment);
}


void l_void()
{
  clearmsg();
  print1(LS(IDS_MSG_21730));
  morewait();
  clearmsg();
  print1(LS(IDS_MSG_21731));
  if (Level->mlist) {
    print2(LS(IDS_MSG_21732));
    print3(LS(IDS_MSG_21733));
  }
  morewait();
  while(Player.hp>0) {
    Time+=60;
    hourly_check();
    usleep(250000);
  }
}


void l_fire_station()
{
  print1(LS(IDS_MSG_21734));
  if (Player.immunity[FLAME]) {
    print2(LS(IDS_MSG_21735));
    morewait();
  }
  print2(LS(IDS_MSG_21736));
  if (ynq2()=='y') {
    if (Player.hp == 1) p_death("total incineration");
    else Player.hp = 1;
    dataprint();
    print1(LS(IDS_MSG_21737));
    if (ynq1()=='y')
      print2(LS(IDS_MSG_21738));
    else {
      Player.pow -= (15+random_range(15));
      if (Player.pow > 0) {
	print2(LS(IDS_MSG_21739));
	print3(LS(IDS_MSG_21740));
	Level->site[Player.x][Player.y].locchar = FLOOR;
	Level->site[Player.x][Player.y].p_locf = L_NO_OP;
	stationcheck();
      }
      else {
	print2(LS(IDS_MSG_21741));
	p_death("the Essence of Fire");
      }
    }
  }
  else print2(LS(IDS_MSG_21742));
}




void l_water_station()
{
  print1(LS(IDS_MSG_21743));
  print2(LS(IDS_MSG_21744));
  morewait();
  if (Player.status[BREATHING]) {
    print1(LS(IDS_MSG_21745));
    morewait();
  }
  if (Player.immunity[ACID]) {
    print2(LS(IDS_MSG_21746));
    morewait();
  }
  print1(LS(IDS_MSG_21747));
  if (ynq1()=='y') {
    if (Player.hp == 1) p_death("drowning in acid (ick, what a way to go)");
    else Player.hp = 1;
    dataprint();
    print2(LS(IDS_MSG_21748));
    morewait();
    nprint2(LS(IDS_MSG_21749));
    morewait();
    print2(LS(IDS_MSG_21750));
    if (ynq2()=='y')
      print2(LS(IDS_MSG_21738));
    else {
      clearmsg();
      Player.con -= (15+random_range(15));
      if (Player.con > 0) {
	print1(LS(IDS_MSG_21751));
	print2(LS(IDS_MSG_21752));
	Level->site[Player.x][Player.y].locchar = FLOOR;
	Level->site[Player.x][Player.y].p_locf = L_NO_OP;
	stationcheck();
      }
      else {
	print2(LS(IDS_MSG_21753));
	p_death("the Essence of Water");
      }
    }
    
  }
  else print2(LS(IDS_MSG_21754));
}




void l_air_station()
{
  print1(LS(IDS_MSG_21755));
  if (Player.immunity[ELECTRICITY])
    print2(LS(IDS_MSG_21756));
  morewait();
  print1(LS(IDS_MSG_21757));
  if (ynq1()=='y') {
    if (Player.hp == 1) p_death("being torn apart and then electrocuted");
    else Player.hp = 1;
    dataprint();
    print1(LS(IDS_MSG_21758));
    print2(LS(IDS_MSG_21759));
    if (ynq1()=='y')
      print2(LS(IDS_MSG_21738));
    else {
      Player.iq -= (random_range(15)+15);
      if (Player.iq > 0) {
	print1(LS(IDS_MSG_21760));
	print2(LS(IDS_MSG_21761));
	Level->site[Player.x][Player.y].locchar = FLOOR;
	Level->site[Player.x][Player.y].p_locf = L_NO_OP;
	stationcheck();
      }
      else {
	print2(LS(IDS_MSG_21762));
	p_death("the Essence of Air");
      }
    }
  }
  else print2(LS(IDS_MSG_21763));
}




void l_earth_station()
{
  pob o;
  print1(LS(IDS_MSG_21764));
  if (find_item(&o,THINGID+6,-1))
    print2(LS(IDS_MSG_21765));
  morewait();
  print1(LS(IDS_MSG_21766));
  if (ynq1()=='y') {
    if (Player.hp == 1) p_death("being eaten alive");
    else Player.hp = 1;
    dataprint();
    print1(LS(IDS_MSG_21767));
    print2(LS(IDS_MSG_21768));
    if (ynq2()=='y')
      print2(LS(IDS_MSG_21738));
    else {
      Player.str -= (15+random_range(15));
      if (Player.str > 0) {
	print1(LS(IDS_MSG_21769));
	print2(LS(IDS_MSG_21770));
	Level->site[Player.x][Player.y].locchar = FLOOR;
	Level->site[Player.x][Player.y].p_locf = L_NO_OP;
	stationcheck();
      }
      else {
	print2(LS(IDS_MSG_21771));
	p_death("the Essence of Earth");
      }
    }
  }
  else print2(LS(IDS_MSG_21772));
}

void stationcheck() 
{
  int stationsleft=FALSE;
  int i,j;
  morewait();
  clearmsg();
  print1(LS(IDS_MSG_21773));
  Player.hp = Player.maxhp;
  dataprint();
  for(i=0;i<WIDTH;i++)
    for(j=0;j<LENGTH;j++)
      if ((Level->site[i][j].locchar == WATER) ||
	  (Level->site[i][j].locchar == HEDGE) ||
	  (Level->site[i][j].locchar == WHIRLWIND) ||
	  (Level->site[i][j].locchar == FIRE))
	stationsleft=TRUE;
  if (! stationsleft) {
    print1(LS(IDS_MSG_21774));
    print2(LS(IDS_MSG_21775));
    print3(LS(IDS_MSG_21776));
    morewait();clearmsg();
    print1(LS(IDS_MSG_21777));
    print2(LS(IDS_MSG_21778));
    print3(LS(IDS_MSG_21779));
    make_site_monster(32,4,ML10+0);
  }
}


/* To survive the void, the other four stations must be visited first,
   to activate the void, then something (Death's scythe, possibly) 
   must be thrown in to satiate the void, then all other items must
   be dropped, then the void must be entered. */

void l_void_station()
{
  int i,something=FALSE;
  print1(LS(IDS_MSG_21780));
  if (ynq()=='y') {
    if (Level->mlist == NULL) {
      print2(LS(IDS_MSG_21781));
      morewait();
      while(Player.hp>0) {
	Time+=60;
	hourly_check();
	usleep(250000);
      }
    }
    else {
      print1(LS(IDS_MSG_21782));
      print2(LS(IDS_MSG_21783));
      morewait();
      something = (Player.packptr > 0);
      if (! something)
	for(i=0;((i<MAXITEMS)&&(!something));i++)
	  if (Player.possessions[i] != NULL)
	    something = TRUE;
      if (something) {
	print1(LS(IDS_MSG_21784));
	print2(LS(IDS_MSG_21785));
	morewait();
	print1(LS(IDS_MSG_21786));
	print2(LS(IDS_MSG_21787));
	p_death("the Power of the Void");
      }
      else if (! gamestatusp(PREPARED_VOID)){
	print1(LS(IDS_MSG_21788));
	print2(LS(IDS_MSG_21789));
	p_death("the Emptyness of the Void");
      }
      else {
	print1(LS(IDS_MSG_21790));
	print2(LS(IDS_MSG_21791));
	print3(LS(IDS_MSG_21792));
	morewait();clearmsg();
	print1(LS(IDS_MSG_21793));
	print2(LS(IDS_MSG_21794));
	print3(LS(IDS_MSG_21795));
	morewait();
	clearmsg();
	print1(LS(IDS_MSG_21796));
	Player.rank[ADEPT] = 1;
	setgamestatus(COMPLETED_CHALLENGE);
	FixedPoints = calc_points();
	/* set so change_environment puts player in correct temple! */
	Player.x = 49;
	Player.y = 59;
	print2(LS(IDS_MSG_21797));
	morewait();
	change_environment(E_TEMPLE);
      }
    }
  }
  else print2(LS(IDS_MSG_21798));
}


void l_voice1()
{
  print1(LS(IDS_MSG_21799));
  Level->site[Player.x][Player.y].p_locf = L_NO_OP;
}

void l_voice2()
{
  print1(LS(IDS_MSG_21800));
  Level->site[Player.x][Player.y].p_locf = L_NO_OP;
}

void l_voice3()
{
  print1(LS(IDS_MSG_21801));
  Level->site[Player.x][Player.y].p_locf = L_NO_OP;
}


void l_whirlwind()
{
  print1(LS(IDS_MSG_21802));
  p_damage(random_range(difficulty()*10),NORMAL_DAMAGE,"a magic whirlwind");
  if (random_range(2)) {
    print2(LS(IDS_MSG_21803));
    p_damage(random_range(difficulty()*10),ELECTRICITY,"a magic whirlwind");
  }
  morewait();
  if (random_range(2)) {
    print1(LS(IDS_MSG_21804));
    if (random_range(20)==17)
      print2(LS(IDS_MSG_21805));
    p_teleport(0);
  }
}


void l_enter_circle()
{
  print1(LS(IDS_MSG_21806));
  print2(LS(IDS_MSG_21807));
  if (ynq()=='y')
    change_environment(E_CIRCLE);
}

void l_circle_library()
{
  print1(LS(IDS_MSG_21808));
}

void l_tome1()
{
  menuclear();
  menuprint("\nYou discover in a dusty tome some interesting information....");
  menuprint("\nThe Star Gem holds a vast amount of mana, usable");
  menuprint("\nfor either Law or Chaos. It is magically linked to Star Peak");
  menuprint("\nand can either be activated or destroyed there. If destroyed,");
  menuprint("\nits power will be used for Chaos, if activated, for Law.");
  menuprint("\n\nIt is said the LawBringer has waited for an eternity");
  menuprint("\nat Star Peak for someone to bring him the gem.");
  menuprint("\nIt is also rumored that while anyone might destroy the gem,");
  menuprint("\nreleasing chaotic energy, only the LawBringer can release");
  menuprint("\nthe lawful potential of the gem.");
  showmenu();
  morewait();
  xredraw();
}


void l_tome2()
{
  menuclear();
  menuprint("\nYou discover in some ancient notes that the Star Gem can be");
  menuprint("\nused for transportation, but also read a caution that it must");
  menuprint("\nbe allowed to recharge a long time between uses.");
  menuprint("\nA marginal note says 'if only it could be reset to go somewhere");
  menuprint("\nbesides Star Peak, the gem might be useful....'");
  showmenu();
  morewait();
  xredraw();
}


void l_temple_warning()
{
  print1(LS(IDS_MSG_21809));
  print2(LS(IDS_MSG_21810));
  print3(LS(IDS_MSG_21811));
}

void l_throne()
{
  pob o;
  int i;
  print1(LS(IDS_MSG_21812));
  print2(LS(IDS_MSG_21813));
  if (ynq1()=='y') {
    if (! find_item(&o,ARTIFACTID+22,-1)) {
      print1(LS(IDS_MSG_21814));
      print2(LS(IDS_MSG_21815));
      print3(LS(IDS_MSG_21816));
      for(i=0;i<NUMSPELLS;i++) Spells[i].known = FALSE;
      Player.pow = 3;
      Player.mana = 0;
      Player.hp = 1;
      dispel(-1);
      morewait();clearmsg();
      print1(LS(IDS_MSG_21817));
    }
    else {
      if (HiMagicUse == Date) 
	print3(LS(IDS_MSG_21818));
      else {
	HiMagicUse = Date;
	print1(LS(IDS_MSG_21819));
	print2(LS(IDS_MSG_21820));
	switch(HiMagic++) {
	case 0: 
	  print3(LS(IDS_MSG_21821));
	  Player.str+=5;
	  Player.maxstr+=5;
	  break;
	case 1: 
	  print3(LS(IDS_MSG_21822));  
	  Player.con+=5;
	  Player.maxcon+=5;
	  break;
	case 2: 
	  print3(LS(IDS_MSG_21823));
	  Player.dex+=5;
	  Player.maxdex+=5;
	  break;
	case 3: 
	  print3(LS(IDS_MSG_21824));  
	  Player.agi+=5;
	  Player.maxagi+=5;
	  break;
	case 4: 
	  print3(LS(IDS_MSG_21825));
	  Player.iq+=5;
	  Player.maxiq+=5;
	  break;
	case 5: 
	  print3(LS(IDS_MSG_21826));  
	  Player.pow+=5;
	  Player.maxpow+=5;
	  break;
	default:
	  if (Spells[S_WISH].known) {
	    print1(LS(IDS_MSG_21827));
	    print2(LS(IDS_MSG_21828));
	  }
	  else {
	    print1(LS(IDS_MSG_21829));
	    print2(LS(IDS_MSG_21830));
	    morewait();
	    print1(LS(IDS_MSG_21831));
	    print2(LS(IDS_MSG_21832));
	    Spells[S_WISH].known = TRUE;
	  }
	  break;
	case 17:
	  print1(LS(IDS_MSG_21833));
	  print2(LS(IDS_MSG_21834));
	  morewait();
	  print1(LS(IDS_MSG_21835));
	  print2(LS(IDS_MSG_21836));
	  Level->site[Player.x][Player.y].locchar = RUBBLE;
	  Level->site[Player.x][Player.y].p_locf = L_RUBBLE;
	  lset(Player.x, Player.y, CHANGED);
	  if (find_and_remove_item(ARTIFACTID+22,-1)) {
	    morewait();
	    print1(LS(IDS_MSG_21837));
	    print2(LS(IDS_MSG_21838));
	  }
	  break;
	}
	calc_melee();
	dataprint();
      }
    }
  }
}


void l_escalator()
{
  print1(LS(IDS_MSG_21839));
  print2(LS(IDS_MSG_21840));
  morewait();
  print1(LS(IDS_MSG_21841));
  if (ynq1()=='y') {
    print1(LS(IDS_MSG_21842));
    print2(LS(IDS_MSG_21843));
    change_environment(E_COUNTRYSIDE);
  }
}

void l_enter_court()
{
  print1(LS(IDS_MSG_21844));
  if (ynq1()=='y') {
    if (! gamestatusp(COMPLETED_CASTLE)) {
      if (! gamestatusp(ATTACKED_ORACLE)) {
	print2(LS(IDS_MSG_21845));
	morewait();
      }
      setgamestatus(COMPLETED_CASTLE);
    }
    change_environment(E_COURT);
  }
}

void l_chaostone()
{
  print1(LS(IDS_MSG_21846));
  print2(LS(IDS_MSG_21847));
  morewait();
  print1(LS(IDS_MSG_21848));
  if (Player.alignment < 0) 
    print2(LS(IDS_MSG_21849));
  else print2(LS(IDS_MSG_21850));
  morewait();
  clearmsg();
  print1(LS(IDS_MSG_21851));
  if (ynq1()=='y') {
    print1(LS(IDS_MSG_21852));
    morewait();
    if (stonecheck(-1)) {
      print2(LS(IDS_MSG_21853));
      Player.maxstr = min(Player.maxstr+10,max(30,Player.maxstr));
      dataprint();
    }
  }
  else print1(LS(IDS_MSG_21854));
}


void l_balancestone()
{
  print1(LS(IDS_MSG_21855));
  print2(LS(IDS_MSG_21856));
  morewait();
  clearmsg();
  print1(LS(IDS_MSG_21851));
  if (ynq1()=='y') {
    print1(LS(IDS_MSG_21857));
    if (abs(Player.alignment) > random_range(50)) {
      print2(LS(IDS_MSG_21858));
      morewait();
      change_environment(E_COUNTRYSIDE);
      do {
	Player.x = random_range(WIDTH);
	Player.y = random_range(LENGTH);
      } while (Country[Player.x][Player.y].current_terrain_type == CHAOS_SEA);
      screencheck(Player.y);
      drawvision(Player.x,Player.y);
    }
    else {
      print2(LS(IDS_MSG_21859));
      if (ynq2()=='y') {
	clearmsg();
	print1(LS(IDS_MSG_21860));
      }
      else {
	Player.xp -= Player.xp/4;
	dataprint();
	print2(LS(IDS_MSG_21861));
	morewait();
	Player.alignment = 0;
	strategic_teleport(1);
      }
    }
  }
  else print1(LS(IDS_MSG_21862));
}


void l_lawstone()
{
  print1(LS(IDS_MSG_21863));
  print2(LS(IDS_MSG_21864));
  morewait();
  print1(LS(IDS_MSG_21865));
  if (Player.alignment > 0) 
    print2(LS(IDS_MSG_21866));
  else print2(LS(IDS_MSG_21867));
  morewait();
  clearmsg();
  print1(LS(IDS_MSG_21851));
  if (ynq()=='y') {
    print1(LS(IDS_MSG_21868));
    morewait();
    if (stonecheck(1)) {
      print2(LS(IDS_MSG_21869));
      Player.maxcon = min(Player.maxcon+10,max(Player.maxcon,30));
      dataprint();
    }
  }
  else print1(LS(IDS_MSG_21870));
}


void l_voidstone()
{
  int i;
  print1(LS(IDS_MSG_21871));
  print2(LS(IDS_MSG_21872));
  morewait();
  clearmsg();
  print1(LS(IDS_MSG_21851));
  if (ynq()=='y') {
    print1(LS(IDS_MSG_21873));
    morewait();
    Player.mana = 0;
    for(i=0;i<NUMSTATI;i++)
      Player.status[i] = 0;
    toggle_item_use(TRUE);
    for(i=0;i<MAXITEMS;i++) 
      if (Player.possessions[i] != NULL) {
	Player.possessions[i]->blessing = 0;
	Player.possessions[i]->plus = 0;
	Player.possessions[i]->usef = I_NOTHING;
      }
    toggle_item_use(FALSE);
    calc_melee();
  }
  else print1(LS(IDS_MSG_21874));
}

void l_sacrificestone()
{
  int sacrifice=1;
  int oldmaxhp = Player.maxhp;
  print1(LS(IDS_MSG_21875));
  print2(LS(IDS_MSG_21876));
  morewait();
  print1(LS(IDS_MSG_21877));
  print2(LS(IDS_MSG_21878));
  if (ynq2() == 'y') {
    print1(LS(IDS_MSG_21879));
    print2(LS(IDS_MSG_21880));
    if (ynq2() == 'y') {
      print1(LS(IDS_MSG_21881));
      print2(LS(IDS_MSG_21882));
      morewait();
      print1(LS(IDS_MSG_21883));
      do {
	switch(random_range(4)) {
	  case 0: print2(LS(IDS_MSG_21884)); break;
	  case 1: print2(LS(IDS_MSG_21885)); break;
	  case 2: print2(LS(IDS_MSG_21886)); break;
	  case 3: print2(LS(IDS_MSG_21887)); break;
	  }
	Player.hp -= sacrifice;
	Player.maxhp -= sacrifice/2;
	sacrifice *= 2;
	dataprint();
	if ((Player.hp < 1) || (Player.maxhp < 1))
	  p_death("self-sacrifice");
      } while (stillonblock());
      print1(LS(IDS_MSG_21888));
      print2(LS(IDS_MSG_21889));
      morewait();
      if ((Player.maxhp > 10) && (Player.maxhp < 3 * oldmaxhp/4)) {
	print1(LS(IDS_MSG_21890));
	print2(LS(IDS_MSG_21891));
	Player.pow += sacrifice;
	Player.maxpow += sacrifice/10;
	dataprint();
      }
      else {
	print1(LS(IDS_MSG_21892));
	print2(LS(IDS_MSG_21893));
	gain_experience(sacrifice);
      }
    }
    else {
      print1(LS(IDS_MSG_21894));
      print2(LS(IDS_MSG_21895));
    }
  }
  else {
    print1(LS(IDS_MSG_21896));
    print2(LS(IDS_MSG_21897));
  }
}

void l_mindstone()
{
  print1(LS(IDS_MSG_21898));
  print2(LS(IDS_MSG_21899));
  morewait();
  print1(LS(IDS_MSG_21900));
  print2(LS(IDS_MSG_21901));
  if (ynq2()=='n') {
    print1(LS(IDS_MSG_21902));
    print2(LS(IDS_MSG_21903));
    morewait();
    if (stonecheck(0)) {
      print1(LS(IDS_MSG_21904));
      Player.maxiq = min(Player.maxiq+10,max(Player.maxiq,30));
      dataprint();
    }
  }
  else {
    print1(LS(IDS_MSG_21905));
    print2(LS(IDS_MSG_21906));
  }
}
