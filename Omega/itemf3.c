/* omega copyright (C) 1987,1988,1989 by Laurence Raphael Brothers */
/* itemf3.c */

/* mostly artifact and stick functions */

#include "glob.h"

/* amulet of the planes */
void i_planes(o)
pob o;
{
  if (Player.mana < 1) print1(LS(IDS_MSG_21496));
  else {
    print1(LS(IDS_MSG_21497));
    Player.mana = max(0,Player.mana-100);
    dataprint();
    morewait();
    strategic_teleport(1);
  }
}


/* the sceptre of high magic */
void i_sceptre(o)
pob o;
{
  if (HiMagicUse == Date) 
    print1(LS(IDS_MSG_21498));
  else {
    HiMagicUse = Date;
    print1(LS(IDS_MSG_21499));
    print2(LS(IDS_MSG_21500));
    if (ynq()=='y') change_environment(E_COURT);
  }
  print1(LS(IDS_MSG_21501));
  print2(LS(IDS_MSG_21502));
  morewait();
}


/* the star gem */
void i_stargem(o)
pob o;
{
  if (StarGemUse == Date) {
    print1(LS(IDS_MSG_21503));
    print2(LS(IDS_MSG_21504));
    print3(LS(IDS_MSG_21505));
    Objects[o->id].uniqueness = UNIQUE_UNMADE;
		/* it's now out there, somewhere */
    dispose_lost_objects(1,o);
  }
  else {
    StarGemUse = Date;
    if (o->blessing < 1) {
      print1(LS(IDS_MSG_21506));
      print2(LS(IDS_MSG_21507));
      morewait();
      o->blessing = 10;
    }
    print1(LS(IDS_MSG_21508));
    morewait();
    if (Player.status[ILLUMINATION] < 1000) {
      print1(LS(IDS_MSG_21509));
      print2(LS(IDS_MSG_21510));
      morewait();
      Player.status[ILLUMINATION] = 1500;
    }
    print1(LS(IDS_MSG_21511));
    morewait();
    setgamestatus(COMPLETED_ASTRAL);
    change_environment(E_COUNTRYSIDE);
    Player.x = 61;
    Player.y = 3;
    screencheck(3);
    drawvision(Player.x,Player.y);
    locprint("Star Peak");
    Country[Player.x][Player.y].current_terrain_type =
      Country[Player.x][Player.y].base_terrain_type;
    c_set(Player.x, Player.y, CHANGED);
    print2(LS(IDS_MSG_21512));
  }
}


/* wand of fear */
void i_fear(o)
pob o;
{
  int x=Player.x,y=Player.y;
  Objects[o->id].known = 1;
  o->known = max(1,o->known);
  setspot(&x,&y);
  if (o->blessing < 0) {
    x = Player.x;
    y = Player.y;
  }
  inflict_fear(x,y);
}

	

void i_juggernaut(o)
pob o;
{
  int d,x=Player.x,y=Player.y;
  int seen = 1, not_seen = 0;
  int tunneled = 0;

  print1(LS(IDS_MSG_21513));
  if (! o->known) {
    print2(LS(IDS_MSG_21514));
    p_death("the Juggernaut of Karnak");
  }
  else {
    d = getdir();
    if (d == ABORT)
      print2(LS(IDS_MSG_21515));
    else {
      print1(LS(IDS_MSG_21516));
      while (inbounds(x+Dirs[0][d],y+Dirs[1][d])) {
	x+=Dirs[0][d];
	y+=Dirs[1][d];
	if (!view_unblocked(x, y) || offscreen(y))
	  seen = 0;
	if (Level->site[x][y].locchar == WALL)
	  tunneled++;
	if (Level->site[x][y].locchar != WATER &&
	  Level->site[x][y].locchar != VOID_CHAR &&
	  Level->site[x][y].locchar != ABYSS &&
	  Level->site[x][y].locchar != SPACE &&
	  Level->site[x][y].locchar != LAVA)
	{
	  Level->site[x][y].locchar = FLOOR;
	  Level->site[x][y].p_locf = L_NO_OP;
	}
	lreset(x, y, SECRET);
	lset(x, y, CHANGED);
	if (Level->site[x][y].creature != NULL) {
	  if (seen)
	    nprint1(LS(IDS_MSG_21517));
	  else
	    not_seen++;
	  setgamestatus(SUPPRESS_PRINTING);
	  m_death(Level->site[x][y].creature);
	  resetgamestatus(SUPPRESS_PRINTING);
	}
	plotspot(x, y, FALSE);
	omshowcursor(x, y);
      }
      if (not_seen > 6)
	print2(LS(IDS_MSG_21518));
      else if (not_seen > 3)
	print2(LS(IDS_MSG_21519));
      else if (not_seen > 1)
	print2(LS(IDS_MSG_21520));
      else if (not_seen == 1)
	print2(LS(IDS_MSG_21521));
      gain_experience(1000);
      dispose_lost_objects(1,o);
      Level->tunnelled += tunneled - 1;
      tunnelcheck();
    }
  }
}


void i_symbol(o)
pob o;
{
  int i;
  if (! o->known)
    print1(LS(IDS_MSG_21522));
  /* if o->charge != 17, then symbol was stolen from own high priest! */
  else if ((o->aux != Player.patron) || (o->charge != 17)){
    print1(LS(IDS_MSG_21523));
    print2(LS(IDS_MSG_21524));
    print3(LS(IDS_MSG_21525));
    morewait();
    for(;Player.hp>1;Player.hp--) {
      dataprint();
      morewait();
      for(i=0;i<MAXITEMS;i++)
	if (Player.possessions[i] != NULL)
	  dispose_lost_objects(Player.possessions[i]->number,
			       Player.possessions[i]);
      Player.mana = 0;
    }
  }
  else if (SymbolUseHour == hour()) {
    print1(LS(IDS_MSG_21526));
    print2(LS(IDS_MSG_21527));
    dispose_lost_objects(1,o);
    Player.hp = 1;
    dataprint();
  }
  else {
    print1(LS(IDS_MSG_21528));
    SymbolUseHour = hour();
    cleanse(1);
    heal(10);
    Player.mana = max(Player.mana,calcmana());
  }
}



void i_crystal(o)
pob o;
{
  if (!o->known) print1(LS(IDS_MSG_21529));
  else {
    print1(LS(IDS_MSG_21530));
    if (ViewHour == hour()) print2(LS(IDS_MSG_21531));
    else if ((o->blessing<0) || (Player.iq+Player.level < random_range(30))) {
      ViewHour = hour();
      print2(LS(IDS_MSG_21532));
      amnesia();
    }
    else {
      ViewHour = hour();
      print2(LS(IDS_MSG_21533));
      mondet(1);
      morewait();
      print2(LS(IDS_MSG_21534));
      objdet(1);
      morewait();
      print2(LS(IDS_MSG_21535));
      hint();
    }
  }
}
    
void i_antioch(o)
pob o;
{
  int x=Player.x,y=Player.y;
  int count;
  if (!o->known){
    print1(LS(IDS_MSG_21536));
    print2(LS(IDS_MSG_21537));
    p_death("the Holy Hand-Grenade of Antioch");
  }
  else {
    print1(LS(IDS_MSG_21538));
    setspot(&x,&y);
    print2(LS(IDS_MSG_21539));
    morewait();
    print1(LS(IDS_MSG_21540));
    count = (int) parsenum();
    if ((count < 3)&&(Level->site[x][y].creature!=NULL)) {
      print1(LS(IDS_MSG_21541));
      print2(LS(IDS_MSG_21542));
      print3(LS(IDS_MSG_21543));
      morewait();
      clearmsg();
      print1(LS(IDS_MSG_21536));
      p_death("the Holy Hand-Grenade of Antioch");
    }
    else if (count>3) {
      print1(LS(IDS_MSG_21544));
      print2(LS(IDS_MSG_21542));
      morewait();
      clearmsg();
      print1(LS(IDS_MSG_21536));
      p_death("the Holy Hand-Grenade of Antioch");
    }
    else {
      print1(LS(IDS_MSG_21536));
      gain_experience(1000);
      Level->site[x][y].locchar = TRAP;
      Level->site[x][y].p_locf = L_TRAP_DOOR;
      lset(x, y, CHANGED);
      if (Level->site[x][y].creature != NULL) {
	m_death(Level->site[x][y].creature);
	print2(LS(IDS_MSG_21545));
      }
      Level->site[x][y].things = NULL;
    }
  }
  dispose_lost_objects(1,o);
}

void i_kolwynia(o)
pob o;
{
  int i;
  if (! o->known) {
    print1(LS(IDS_MSG_21546));
    p_death("Kolwynia, The Key That Was Lost");
  }
  else {
    gain_experience(5000);
    print1(LS(IDS_MSG_21547));
    Player.pow = Player.maxpow = 2*Player.maxpow;
    for(i=0;i<NUMSPELLS;i++)
      Spells[i].known = TRUE;
  }
  dispose_lost_objects(1,o);
}

void i_enchantment(o)
pob o;
{
  char response;
  if (ZapHour == hour()) 
    print1(LS(IDS_MSG_21548));
  else if (! o->known) {
    ZapHour = hour();
    print1(LS(IDS_MSG_21549));
    dispel(-1);
  }
  else {
    ZapHour = hour();
    print1(LS(IDS_MSG_21550));
    do response = (char) mcigetc();
    while ((response != 'w') && (response != 'b'));
    print2(LS(IDS_MSG_21551));
    if (response == 'w') enchant(o->blessing*2+1);
    else dispel(o->blessing);
  }
}

void i_helm(o)
pob o;
{
  if (HelmHour == hour()) 
    print1(LS(IDS_MSG_21552));
  else if (! o->known) {
    HelmHour = hour();
    print1(LS(IDS_MSG_21553));
    p_teleport(-1);
  }
  else {
    HelmHour = hour();
    print1(LS(IDS_MSG_21554));
    p_teleport(o->blessing);
  }
}
 

void i_death(o)
pob o;
{
  clearmsg();
  print1(LS(IDS_MSG_21555));
  p_death("the Potion of Death");
}

void i_life(o)
pob o;
{
  clearmsg();
  print1(LS(IDS_MSG_21556));
  Player.hp = Player.maxhp = 2 * Player.maxhp;
  dispose_lost_objects(1,o);
}



/* f = fire, w = water, e = earth, a = air, m = mastery */
int orbcheck(element)
char element;
{
  char response;
  print1(LS(IDS_MSG_21557));
  print2(LS(IDS_MSG_21558));
  morewait();
  print1(LS(IDS_MSG_21559));
  print2(LS(IDS_MSG_21560));
  morewait();
  print1(LS(IDS_MSG_21561));
  print2(LS(IDS_MSG_21562));
  morewait();
  print1(LS(IDS_MSG_21563));
  do {
    print2(LS(IDS_MSG_21564));
    response = (char) mcigetc();
  } while ((response != 'f') &&
	   (response != 'w') &&
	   (response != 'e') &&
	   (response != 'a') &&
	   (response != 'm'));
  return(response == element);
}

/* orb functions */
void i_orbfire(o)
pob o;
{
  if (! orbcheck('f')) {
    print1(LS(IDS_MSG_21565));
    print2(LS(IDS_MSG_21566));
    fball(Player.x,Player.y,Player.x,Player.y,250);
    o->known = 1;
  }
  else {
    print1(LS(IDS_MSG_21567));
    Spells[S_FIREBOLT].known = TRUE;
    gain_experience(10000);
    Player.immunity[FLAME]+=100;
    print2(LS(IDS_MSG_21568));
    o->plus = 100;
    o->blessing = 100;
    i_firebolt(o);
  }
  *o = Objects[ARTIFACTID+5];
}


void i_orbwater(o)
pob o;
{
  if (! orbcheck('w')) {
    print1(LS(IDS_MSG_21569));
    print2(LS(IDS_MSG_21570));
    heal(-250);
    o->known = 1;
  }
  else {
    print1(LS(IDS_MSG_21571));
    Spells[S_DISRUPT].known = TRUE;
    Player.immunity[POISON]+=100;
    gain_experience(10000);
    print2(LS(IDS_MSG_21572));
    o->plus = 100;
    o->blessing = 100;
    i_disrupt(o);
  }
  *o = Objects[ARTIFACTID+5];
}






void i_orbearth(o)
pob o;
{
  int i;
  if (! orbcheck('e')) {
    print1(LS(IDS_MSG_21573));
    print2(LS(IDS_MSG_21574));
    Player.con -= 10;
    if (Player.con < 3)
      p_death("congestive heart failure");
    else {
      print3(LS(IDS_MSG_21575));
      for (i=0;i<MAXITEMS;i++)
	if (Player.possessions[i] != NULL) 
	  dispose_lost_objects(Player.possessions[i]->number,
			       Player.possessions[i]);
      for (i=0;i<MAXPACK;i++) 
	if (Player.pack[i] != NULL) {
	  free((char *) Player.pack[i]);
	  Player.pack[i] = NULL;
	}
      Player.packptr = 0;
      o->known = 1;
    }
  }
  else {
    print1(LS(IDS_MSG_21576));
    Spells[S_DISINTEGRATE].known = TRUE;
    gain_experience(10000);
    Player.immunity[NEGENERGY]+=100;
    print2(LS(IDS_MSG_21577));
    o->plus = 100;
    o->blessing = 100;
    i_disintegrate(o);
  }
  *o = Objects[ARTIFACTID+5];
}


void i_orbair(o)
pob o;
{
  if (! orbcheck('a')) {
    print1(LS(IDS_MSG_21578));
    print2(LS(IDS_MSG_21579));
    lball(Player.x,Player.y,Player.x,Player.y,100);
    o->known = 1;
  }
  else {
    print1(LS(IDS_MSG_21580));
    Spells[S_LBALL].known = TRUE; /* lball */
    gain_experience(10000);
    print2(LS(IDS_MSG_21581));
    Player.immunity[ELECTRICITY]+=100;
    o->plus = 100;
    o->blessing = 100;
    i_invisible(o);
    i_lbolt(o);
  }
  *o = Objects[ARTIFACTID+5];
}


void i_orbmastery(o)
pob o;
{

  if (! orbcheck('m')) {
    print1(LS(IDS_MSG_21582));
    print2(LS(IDS_MSG_21583));
    p_death("playing with the Orb of Mastery");
    o->known = 1;
  }
  else if ((find_and_remove_item(ARTIFACTID+1,-1)) &&
	   (find_and_remove_item(ARTIFACTID+2,-1)) &&
	   (find_and_remove_item(ARTIFACTID+3,-1)) &&
	   (find_and_remove_item(ARTIFACTID+4,-1))) {
    print1(LS(IDS_MSG_21584));
    print2(LS(IDS_MSG_21585));
    Player.iq = Player.maxiq = 2 * Player.maxiq;
    Player.pow = Player.maxpow = 2 * Player.maxpow;
    Player.str = Player.maxstr = 2 * Player.maxstr;
    Player.dex = Player.maxdex = 2 * Player.maxdex;
    Player.con = Player.maxcon = 2 * Player.maxcon;
    Player.agi = Player.maxagi = 2 * Player.maxagi;
    dataprint();
    morewait();
    print1(LS(IDS_MSG_21586));
    morewait();
    wish(1);
    morewait();
    clearmsg();
    print2(LS(IDS_MSG_21587));
    gain_experience(20000);
    *o = Objects[ARTIFACTID+5];
  }
  else {
    print1(LS(IDS_MSG_21588));
    print2(LS(IDS_MSG_21583));
    p_death("playing with the Orb of Mastery");
  }  
}


void i_orbdead(o)
pob o;
{
  int i;
  print1(LS(IDS_MSG_21589));
  for(i=0;i<NUMSPELLS;i++)
    Spells[i].known = FALSE;
  print2(LS(IDS_MSG_21590));
  for(i=0;i<MAXITEMS;i++) {
    if (Player.possessions[i] != NULL) {
      Player.possessions[i]->plus = 0;
      if (Player.possessions[i]->usef > 100)
	Player.possessions[i]->usef = I_NOTHING;
    }
  }
  print3(LS(IDS_MSG_21591));
  level_drain(Player.level-1,"a Burnt-out Orb");
  Player.mana = 0;
  Player.pow -= 10;
}



void i_dispel(o)
pob o;
{
  dispel((o->blessing > -1) ? o->blessing+random_range(3): o->blessing);
}


/* stick functions */


/* wand of apportation */
void i_apport(o)
pob o;
{
  o->known = max(1,o->known);
  Objects[o->id].known = 1;
  apport(o->blessing);
}

/* staff of firebolts */
void i_firebolt(o)
pob o;
{
  int x=Player.x,y=Player.y;
  o->known = max(1,o->known);
  Objects[o->id].known = 1;
  setspot(&x,&y);
  if (o->blessing < 0) {
    x = Player.x;
    y = Player.y;
  }
  fbolt(Player.x,Player.y,x,y,Player.dex*2+Player.level,75);
}


void i_disintegrate(o)
pob o;
{
  int x=Player.x,y=Player.y;
  o->known = max(1,o->known);
  Objects[o->id].known = 1;
  setspot(&x,&y);
  if (o->blessing < 0) {
    x = Player.x;
    y = Player.y;
  }
  disintegrate(x,y);
}

void i_disrupt(o)
pob o;
{
  int x=Player.x,y=Player.y;
  o->known = max(1,o->known);
  Objects[o->id].known = 1;
  setspot(&x,&y);
  if (o->blessing < 0) {
    x = Player.x;
    y = Player.y;
  }
  disrupt(x,y,100);
}


/* staff of lightning bolts */
void i_lbolt(o)
pob o;
{
  int x=Player.x,y=Player.y;
  o->known = max(1,o->known);
  Objects[o->id].known = 1;
  setspot(&x,&y);
  if (o->blessing < 0) {
    x = Player.x;
    y = Player.y;
  }
  lbolt(Player.x,Player.y,x,y,Player.dex*2+Player.level,75);
}

/* wand of magic missiles */
void i_missile(o)
pob o;
{
  int x=Player.x,y=Player.y;
  o->known = max(1,o->known);
  Objects[o->id].known = 1;
  setspot(&x,&y);
  if (o->blessing < 0) {
    x = Player.x;
    y = Player.y;
  }
  nbolt(Player.x,Player.y,x,y,Player.dex*2+Player.level,20);
}

/* wand of fire balls */
void i_fireball(o)
pob o;
{
  int x=Player.x,y=Player.y;
  Objects[o->id].known = 1;
  o->known = max(1,o->known);
  setspot(&x,&y);
  if (o->blessing < 0) {
    x = Player.x;
    y = Player.y;
  }
  fball(Player.x,Player.y,x,y,35);
}


/* wand of snowballs */
void i_snowball(o)
pob o;
{
  int x=Player.x,y=Player.y;
  Objects[o->id].known = 1;
  o->known = max(1,o->known);
  setspot(&x,&y);
  if (o->blessing < 0) {
    x = Player.x;
    y = Player.y;
  }
  snowball(Player.x,Player.y,x,y,20);
}

/* wand of lightning balls */
void i_lball(o)
pob o;
{
  int x=Player.x,y=Player.y;
  Objects[o->id].known = 1;
  o->known = max(1,o->known);
  setspot(&x,&y);
  if (o->blessing < 0) {
    x = Player.x;
    y = Player.y;
  }
  lball(Player.x,Player.y,x,y,50);
}

/* staff of sleep */
void i_sleep_other(o)
pob o;
{
  Objects[o->id].known = 1;
  o->known = max(1,o->known);
  sleep_monster(o->blessing);
}

/* rod of summoning */
/* rod of summoning now always summons as if cursed */
void i_summon(o)
pob o;
{
  Objects[o->id].known = 1;
  o->known = max(1,o->known);
  summon(-1,-1);
}

void i_hide(o)
pob o;
{
  int x=Player.x,y=Player.y;
  Objects[o->id].known = 1;
  o->known = max(1,o->known);
  setspot(&x,&y);
  hide(x,y);
}

void i_polymorph(o)
pob o;
{
  Objects[o->id].known = 1;
  o->known = max(1,o->known);
  polymorph(o->blessing);
}
