/* omega copyright (C) by Laurence Raphael Brothers, 1987,1988,1989 */
/* effect3.c */

#include "glob.h"

/* if know id, then summon that monster; else (if < 0) get one. */
void summon(blessing,id)
int blessing,id;
{
  int i,looking=TRUE,x,y;
  pml tml;

  if (id < 0) {
    if (blessing > 0) {
      id = monsterlist();
      xredraw();
    }
    /* for (id ==0) case, see below -- get a "fair" monster */
    else if (blessing < 0) id = random_range(NUMMONSTERS);
  }
  for(i=0;((i<8) && looking);i++) {
    x = Player.x+Dirs[0][i];
    y = Player.y+Dirs[1][i];
    looking = ((! inbounds(x,y)) ||
	       (Level->site[x][y].locchar != FLOOR) ||
	       (Level->site[x][y].creature != NULL));
  }

  if (! looking) {
    if ((blessing == 0) && (id < 0))
      Level->site[x][y].creature = m_create(x,y,WANDERING,difficulty());
    else Level->site[x][y].creature = make_creature(id);
    Level->site[x][y].creature->x = x;
    Level->site[x][y].creature->y = y;
    tml = ((pml) checkmalloc(sizeof(mltype)));
    tml->m = Level->site[x][y].creature;
    if (blessing > 0)
      m_status_reset(tml->m,HOSTILE);
    else if (blessing < 0)
      m_status_set(tml->m,HOSTILE);
    tml->next = Level->mlist;
    Level->mlist = tml;
  }
}



int itemlist(itemindex,num)
int itemindex,num;
{
  int i,itemno;

  print2(LS(IDS_MSG_20804));
  if (ynq2() == 'y') {
    menuclear();
    for(i=0;i<num;i++) {
      menunumprint(i+1);
      menuprint(":");
      menuprint(Objects[i+itemindex].truename);
      menuprint("\n");
    }
    showmenu();
  }
  mprint(LS(IDS_MSG_20805));
  itemno = (int) parsenum()-1;
  if ((itemno >= num)||(itemno<0)) itemno = ABORT;
  return(itemno);
}

int monsterlist()
{
  int i,itemno;
  print2(LS(IDS_MSG_20804));
  if (ynq2() == 'y')
    do {
      clearmsg();
      print1(LS(IDS_MSG_20806));
      menuclear();
      for(i=0;i<NUMMONSTERS;i++) {
	menunumprint(i+1);
	menuprint(":");
	menuprint(Monsters[i].monstring);
	menuprint("\n");
      }
      showmenu();
      itemno = (int) parsenum()-1;
      if ((itemno < 0) || (itemno > NUMMONSTERS-1)) {
	print3(LS(IDS_MSG_20807));
	morewait();
      }
    } while ((itemno < 0) || (itemno > NUMMONSTERS-1));
  else
    do {
      print1(LS(IDS_MSG_20806));
      itemno = (int) parsenum()-1;
    } while ((itemno < 0) || (itemno > NUMMONSTERS-1));
  return(itemno);
}
      


/* uncurse all items, cure diseases, and neutralize poison */
void cleanse(blessing)
int blessing;
{
  int i;

  if (blessing > -1) {
    if (blessing > 0)
      for(i=0;i<MAXITEMS;i++) 
	if (Player.possessions[i] != NULL) {
	  if ((Player.possessions[i]->used) &&
	      (Player.possessions[i]->blessing < 0)) {
	    Player.possessions[i]->used = FALSE;
	    item_use(Player.possessions[i]);
	    Player.possessions[i]->blessing = 0;
	    Player.possessions[i]->used = TRUE;
	    item_use(Player.possessions[i]);
	  }
	}
    
    if (Player.status[POISONED] > 0) {
      Player.status[POISONED] = 0;
    }
    if (Player.status[DISEASED] > 0) {
      Player.status[DISEASED] = 0;
    }
    showflags();
    mprint(LS(IDS_MSG_20808));
  }
  else {
    Player.status[POISONED] += 10;
    Player.status[DISEASED] += 10;
    mprint(LS(IDS_MSG_20809));
    showflags();
  }
}

void annihilate(blessing)
int blessing;
{
  pml ml;
  int i;

  if (blessing == 0) {
    mprint(LS(IDS_MSG_20810));
    for(i=0;i<9;i++)
      if (Level->site[Player.x+Dirs[0][i]][Player.y+Dirs[1][i]].creature !=
	  NULL)
	m_death(Level->site[Player.x+Dirs[0][i]][Player.y+Dirs[1][i]].creature);
  }
  if (blessing > 0) {
    if (Current_Environment == E_COUNTRYSIDE) {
	clearmsg();
	print1(LS(IDS_MSG_20811));
	morewait();
	print1(LS(IDS_MSG_20812));
	print2(LS(IDS_MSG_20813));
	Player.alignment -= 3;
    }
    else {
      mprint(LS(IDS_MSG_20814));
      for(ml=Level->mlist;ml!=NULL;ml=ml->next)
	if (ml->m != NULL && ml->m->hp > 0)
	  m_death(ml->m);
    }
  }
  else {
    mprint(LS(IDS_MSG_20815));
    p_death("self-annihilation");
  }
}




void sleep_monster(blessing)
int blessing;
{
  pml ml;
  int x=Player.x,y=Player.y;
  struct monster *target;

  if (blessing == 0) setspot(&x,&y);

  if (blessing < 0)
    sleep_player(abs(blessing)+2);
  else if (blessing > 0) {
    mprint(LS(IDS_MSG_20816));
    for (ml=Level->mlist;ml!=NULL;ml=ml->next) {
      m_status_reset(ml->m,AWAKE);
      ml->m->wakeup = 0;
    }
  }
  else {
    target = Level->site[x][y].creature;
    if (target != NULL) {
      if (target->uniqueness == COMMON) {
	strcpy(Str1,LS(IDS_MSG_23120));
	strcat(Str1,target->monstring);
      }
      else strcpy(Str1,target->monstring);
      if (! m_immunityp(target,SLEEP)) {
	strcat(Str1,LS(IDS_MSG_23121));
	m_status_reset(target,AWAKE);
	target->wakeup = 0;
      }
      else strcat(Str1,LS(IDS_MSG_23122));
      mprint(Str1);
    }
    else mprint(LS(IDS_MSG_20817));
  }
}
  
void sleep_player(amount)
int amount;
{
  if (Player.status[SLEPT] == 0) { /* prevent player from sleeping forever */
    mprint(LS(IDS_MSG_20818));
    if (! p_immune(SLEEP)) {
      Player.status[SLEPT] += random_range(amount*2)+2;
    }
    else mprint(LS(IDS_MSG_20819));
  }
}


void hide(x,y)
int x,y;
{
  if (inbounds(x,y)) {
    lset(x,y,SECRET);
    lset(x, y, CHANGED);
    putspot(x, y, WALL);
    mprint(LS(IDS_MSG_20820));
  }
}

void clairvoyance(vision)
int vision;
{
  int i,j;
  int x = Player.x, y = Player.y;
  mprint(LS(IDS_MSG_20821));
  setspot(&x,&y);
  for(i=x-vision;i<x+vision+1;i++)
    for(j=y-vision;j<y+vision+1;j++) {
      if (inbounds(i,j)) {
	Level->site[i][j].showchar = SPACE;
	lreset(i,j,SECRET);
	lset(i, j, CHANGED);
	dodrawspot(i,j);
      }
    }
  levelrefresh();
}

void aggravate()
{
  pml tm;

  for (tm=Level->mlist;tm!=NULL;tm=tm->next){
    m_status_set(tm->m,AWAKE);
    m_status_set(tm->m,HOSTILE);
  }
}




void learnspell(blessing)
int blessing;
{
  int i,spell,done=FALSE;
  if (blessing < 0) {
    for(i=NUMSPELLS;((i>-1) && (! done));i--)
      if (Spells[i].known) {
	done = TRUE;
	Objects[SCROLLID+1].known = TRUE;
	mprint(LS(IDS_MSG_20279));
	Spells[i].known = FALSE;
      }
    if (i == ABORT)
      mprint(LS(IDS_MSG_20085));
  }
  else {
    Objects[SCROLLID+1].known = TRUE;
    spell = random_range(NUMSPELLS);
    print1(LS(IDS_MSG_20822));
    if ((random_range(4*Spells[spell].powerdrain)+
	 Spells[spell].powerdrain) <
	(4*Player.iq+8*Player.level)) {
      nprint1(LS(IDS_MSG_20823));
      nprint1(spellid(spell));
      if (Spells[spell].known) {
	print2(LS(IDS_MSG_20824));
	Spells[spell].powerdrain = ((int) ((Spells[spell].powerdrain+1)/2));
      }
      else {
	print2(LS(IDS_MSG_20825));
	Spells[spell].known = TRUE;
	gain_experience(Spells[spell].powerdrain*10);
      }
    }
    else nprint1(LS(IDS_MSG_20826));
  }
}


void amnesia()
{
  int i,j;
  for (j=0;j<LENGTH;j++)
    for (i=0;i<WIDTH;i++)
      lreset(i,j,SEEN);

  erase_level();
  drawvision(Player.x,Player.y);
}


/*affects player only */
void level_drain(levels,source)
int levels;
char *source;
{
  int decrement = ((int) (Player.maxhp / (Player.level+1)));

  Player.level -= levels;

  Player.maxhp -= (levels * decrement);
  Player.hp -= (levels * decrement);

  if ((Player.hp < 1) || (Player.level < 0))
    p_death(source);
}




void disrupt(x,y,amount)
int x,y,amount;
{
  struct monster *target;

  if ((x ==Player.x) && (y==Player.y)) {
    mprint(LS(IDS_MSG_20827));
    p_damage(amount,NORMAL_DAMAGE,"magical disruption");
  }
  else {
    target = Level->site[x][y].creature;
    if (target != NULL) {
      if (target->uniqueness == COMMON) {
	strcpy(Str1,LS(IDS_MSG_23120));
	strcat(Str1,target->monstring);
      }
      else strcpy(Str1,target->monstring);
      if (! m_immunityp(target,NORMAL_DAMAGE)) {
	strcat(Str1,LS(IDS_MSG_23123));
	mprint(Str1);
	m_damage(target,amount,NORMAL_DAMAGE);
	target->wakeup = 0;
      }
      else {
	strcat(Str1,LS(IDS_MSG_23124));
	mprint(Str1);
      }
    }
  }
}




void disintegrate(x,y)
int x,y;
{
  struct monster *target;
  if (! inbounds(x,y)) mprint(LS(IDS_MSG_20828));
  else if ((x==Player.x)&&(y==Player.y)) {
    if (Player.possessions[O_CLOAK] != NULL) {
      mprint(LS(IDS_MSG_20829));
      dispose_lost_objects(1,Player.possessions[O_CLOAK]);
    }
    else if (Player.possessions[O_ARMOR] != NULL) {
      mprint(LS(IDS_MSG_20830));
      dispose_lost_objects(1,Player.possessions[O_ARMOR]);
    }
    else {
      mprint(LS(IDS_MSG_20831));
      mprint(LS(IDS_MSG_20832));
      p_damage(250,UNSTOPPABLE,"disintegration");
    }
  }
  else {
    if (!view_los_p(Player.x, Player.y, x, y))
      setgamestatus(SUPPRESS_PRINTING);
    if ((target = Level->site[x][y].creature) != NULL) {
      if (target->uniqueness == COMMON) {
	strcpy(Str1,LS(IDS_MSG_23120));
	strcat(Str1,target->monstring);
      }
      else strcpy(Str1,target->monstring);
      strcat(Str1,LS(IDS_MSG_23125));
      mprint(Str1);
      m_damage(target,100,UNSTOPPABLE);
      if (target->hp > 0) mprint(LS(IDS_MSG_20833));
    }
    else if (Level->site[x][y].locchar == ALTAR) {
      mprint(LS(IDS_MSG_20834));
      mprint(LS(IDS_MSG_20835));
      disintegrate(Player.x,Player.y);
    }
    else if (Level->site[x][y].p_locf == L_TRAP_PIT) {
      if (Current_Environment == Current_Dungeon) {
	mprint(LS(IDS_MSG_20836));
	Level->site[x][y].locchar = TRAP;
	Level->site[x][y].p_locf = L_TRAP_DOOR;
	Level->site[x][y].aux = S_DISINTEGRATE;
	lset(x, y, CHANGED);
      }
      else mprint(LS(IDS_MSG_20837));
    }
    else if (Level->site[x][y].locchar == FLOOR) {
      mprint(LS(IDS_MSG_20838));
      Level->site[x][y].locchar = TRAP;
      Level->site[x][y].p_locf = L_TRAP_PIT;
      lset(x, y, CHANGED);
    }
    else if ((Level->site[x][y].locchar == WALL) ||
	     (Level->site[x][y].locchar == OPEN_DOOR) ||
	     (Level->site[x][y].locchar == CLOSED_DOOR) ||
	     (Level->site[x][y].locchar == PORTCULLIS) ||
	     (Level->site[x][y].locchar == STATUE)) {
      mprint(LS(IDS_MSG_20839));
      if (Level->site[x][y].locchar == WALL)
	tunnelcheck();
      Level->site[x][y].p_locf = L_RUBBLE;
      Level->site[x][y].locchar = RUBBLE;
      lreset(x,y,SECRET);
      lset(x, y, CHANGED);
    }
    else if ((Level->site[x][y].locchar == RUBBLE) ||
	     (Level->site[x][y].locchar == TRAP)) {
      mprint(LS(IDS_MSG_20840));
      Level->site[x][y].p_locf = L_NO_OP;
      Level->site[x][y].locchar = FLOOR;
      lreset(x,y,SECRET);
      lset(x, y, CHANGED);
    }
    else if (Level->site[x][y].locchar == HEDGE) {
      if (Level->site[x][y].p_locf == L_TRIFID) {
	mprint(LS(IDS_MSG_20841));
	gain_experience(50);
	Level->site[x][y].p_locf = L_NO_OP;
	Level->site[x][y].locchar = FLOOR;
	lreset(x,y,SECRET);
	lset(x, y, CHANGED);
      }
      else {
	mprint(LS(IDS_MSG_20730));
	Level->site[x][y].p_locf = L_NO_OP;
	Level->site[x][y].locchar = FLOOR;
	lreset(x,y,SECRET);
	lset(x, y, CHANGED);
      }
    }
    else mprint(LS(IDS_MSG_20842));
    if (!view_los_p(Player.x, Player.y, x, y))
      resetgamestatus(SUPPRESS_PRINTING);
    else
      plotspot(x, y, TRUE);
  }
}

void acid_cloud()
{
  mprint(LS(IDS_MSG_20843));
  if (Player.possessions[O_CLOAK] != NULL) {
    (void) damage_item(Player.possessions[O_CLOAK]);
    mprint(LS(IDS_MSG_20844));
    p_damage(3,ACID,"an acid cloud");
  }
  else if (Player.possessions[O_ARMOR] != NULL) {
    mprint(LS(IDS_MSG_20844));
    p_damage(3,ACID,"an acid cloud");
    (void) damage_item(Player.possessions[O_ARMOR]);
  }
  else if (p_immune(ACID))
  {
    mprint(LS(IDS_MSG_20029));
    return;
  }
  else {
    mprint(LS(IDS_MSG_20845));
    p_damage(25,ACID,"an acid cloud");
  }
}



/* teleport player */
void p_teleport(type)
int type;
{
  int x=Player.x,y=Player.y;
  drawspot(x,y);
  if (type < 0) {
    x = random_range(WIDTH);
    y = random_range(LENGTH);
    if ((Level->site[x][y].locchar != FLOOR) &&
	(Level->site[x][y].locchar != OPEN_DOOR)) {
      mprint(LS(IDS_MSG_20846));
      mprint(LS(IDS_MSG_20847));
      p_death("teleportation into a solid object");
    }
    else {
      Player.x = x;
      Player.y = y;
    }
  }
  else if (type == 0)
    findspace(&(Player.x),&(Player.y),-1);
  else {
    setspot(&Player.x,&Player.y);
    if ((Level->site[Player.x][Player.y].locchar != FLOOR) ||
	(Level->site[Player.x][Player.y].creature != NULL)) {
      mprint(LS(IDS_MSG_20848));
      p_teleport(0);
    }
  }
  screencheck(Player.y);
  roomcheck();
}


void p_poison(toxicity)
int toxicity;
{
  mprint(LS(IDS_MSG_20849));
  if (! p_immune(POISON))
    Player.status[POISONED]+=toxicity;
  else mprint(LS(IDS_MSG_20850));
  showflags();
}

void apport(blessing)
int blessing;
{
  int i,index,x=Player.x,y=Player.y;
  if (blessing > -1) {
    mprint(LS(IDS_MSG_20851));
    setspot(&x,&y);
    if (Level->site[x][y].things != NULL) {
      pickup_at(x,y);
      plotspot(x, y, TRUE);
    }
    else mprint(LS(IDS_MSG_20852));
  }
  else {
    mprint(LS(IDS_MSG_20853));
    for(i=0;i<abs(blessing);i++) {
      index = random_item();
      if (index != ABORT) {
	drop_at(x,y,Player.possessions[index]);
	dispose_lost_objects(Player.possessions[index]->number,
	  Player.possessions[index]);
      }
    }
  }
}


void strategic_teleport(blessing)
int blessing;
{
  int new_env;

  mprint(LS(IDS_MSG_20854));
  if (blessing < 0) {
    morewait();
    mprint(LS(IDS_MSG_20855));
    change_environment(E_COUNTRYSIDE);
    do {
      Player.x = random_range(WIDTH);
      Player.y = random_range(LENGTH);
    } while(Country[Player.x][Player.y].base_terrain_type == CHAOS_SEA);
  }
  else {
    mprint(LS(IDS_MSG_20856));
    menuclear();
    menuprint("a: Rampart\n");
    menuprint("b: Village of Star View\n");
    menuprint("c: Village of Woodmere\n");
    menuprint("d: Village of Stormwatch\n");
    menuprint("e: Village of Thaumaris\n");
    menuprint("f: Village of Skorch\n");
    menuprint("g: Village of Whorfen\n");
    menuprint("h: Temple of the Noose\n");
    menuprint("i: The Parthenon\n");
    menuprint("j: Temple of the Black Hand\n");
    menuprint("k: Temple of the Hidden Moon\n");
    menuprint("l: WoodHenge\n");
    menuprint("m: Temple of Destiny\n");
    menuprint("n: HellWell Volcano\n");
    menuprint("ANYTHING ELSE: Avoid entering a portal.");
    showmenu();
    switch((char) mcigetc()) {
    case 'a': 
      change_environment(E_COUNTRYSIDE); 
      Player.x = 27;
      Player.y = 19;
      break;
    case 'b':
      change_environment(E_COUNTRYSIDE);
      Player.x = 56;
      Player.y = 5;
      break;
    case 'c':
      change_environment(E_COUNTRYSIDE);
      Player.x = 35;
      Player.y = 11;
      break;
    case 'd':
      change_environment(E_COUNTRYSIDE);
      Player.x = 10;
      Player.y = 40;
      break;
    case 'e':
      change_environment(E_COUNTRYSIDE);
      Player.x = 7;
      Player.y = 6;
      break;
    case 'f':
      change_environment(E_COUNTRYSIDE);
      Player.x = 41;
      Player.y = 43;
      break;
    case 'g':
      change_environment(E_COUNTRYSIDE);
      Player.x = 20;
      Player.y = 41;
      break;
    case 'h':
      change_environment(E_COUNTRYSIDE);
      Player.x = 22;
      Player.y = 30;
      break;
    case 'i':
      change_environment(E_COUNTRYSIDE);
      Player.x = 51;
      Player.y = 11;
      break;
    case 'j':
      change_environment(E_COUNTRYSIDE);
      Player.x = 45;
      Player.y = 45;
      break;
    case 'k':
      change_environment(E_COUNTRYSIDE);
      Player.x = 19;
      Player.y = 46;
      break;
    case 'l':
      change_environment(E_COUNTRYSIDE);
      Player.x = 32;
      Player.y = 5;
      break;
    case 'm':
      change_environment(E_COUNTRYSIDE);
      Player.x = 49;
      Player.y = 59;
      break;
    case 'n':
      change_environment(E_COUNTRYSIDE);
      Player.x = 30;
      Player.y = 58;
      break;
    default:
      if (gamestatusp(CHEATED)) {
	mprint(LS(IDS_MSG_20857));
	new_env = (int) parsenum();
	change_environment(new_env);
      }
    }
    xredraw();
    if (gamestatusp(LOST)) {
      print1(LS(IDS_MSG_20214));
      resetgamestatus(LOST);
      Precipitation = 0;
    }
  }
  setlastxy(Player.x, Player.y);
  screencheck(Player.y);
  drawvision(Player.x,Player.y);
  if (Current_Environment == E_COUNTRYSIDE)
    terrain_check(FALSE);
}



void hero(blessing)
int blessing;
{
  if (blessing > -1) {
      mprint(LS(IDS_MSG_20858));
      Player.status[HERO] += random_range(5)+1+blessing;
      calc_melee();
    }
  else {
    Player.status[HERO]=0;
    calc_melee();
    mprint(LS(IDS_MSG_20859));
    level_drain(abs(blessing),"a potion of cowardice");
  }
}


void levitate(blessing)
int blessing;
{
  if (blessing > -1) {
    if (gamestatusp(MOUNTED)) 
      mprint(LS(IDS_MSG_20860));
    else {
      mprint(LS(IDS_MSG_20861));
      mprint(LS(IDS_MSG_20862));
      mprint(LS(IDS_MSG_20863));
      Player.status[LEVITATING] += random_range(5)+1+blessing;
    }
  }
  else mprint(LS(IDS_MSG_20778));
}


/* has effect of switching between 1st level and deepest level attained */
void level_return()
{
  if (Current_Environment == Current_Dungeon) {
    mprint(LS(IDS_MSG_20864));
    if (Level->depth > 1)
      change_level(Level->depth,1,FALSE);
    else change_level(Level->depth,deepest[Current_Environment],FALSE);
  }
  else if (Current_Environment == E_COUNTRYSIDE) {
    mprint(LS(IDS_MSG_20865));
    Player.x = 27;
    Player.y = 19;
    screencheck(Player.y);
    drawvision(Player.x,Player.y);
    locprint("Back Outside Rampart.");
  }
  else mprint(LS(IDS_MSG_20866));
}


void cure(blessing)
int blessing;
{
  int happened = FALSE;
  if (blessing > -1) {
    if (Player.status[DISEASED]) {
      Player.status[DISEASED]=0;
      mprint(LS(IDS_MSG_20867));
      happened = TRUE;
    }
    if (Player.status[POISONED]) {
      Player.status[POISONED] -= 5+blessing*10;
      if (Player.status[POISONED] > 0)
	mprint(LS(IDS_MSG_20868));
      else {
	Player.status[POISONED] = 0;
	mprint(LS(IDS_MSG_20869));
      }
      happened = TRUE;
    }
    if (Player.status[BLINDED]) {
      Player.status[BLINDED]=0;
      happened = TRUE;
      mprint(LS(IDS_MSG_20870));
    }
    if (! happened) mprint(LS(IDS_MSG_20778));
  }
  else disease(12);
  showflags();
}

void disease(amount)
int amount;
{
  mprint(LS(IDS_MSG_20871));
  if (! Player.immunity[INFECTION]) {
    mprint(LS(IDS_MSG_20872));
    Player.status[DISEASED]+=random_range(amount*2)+1;
  }
  else mprint(LS(IDS_MSG_20873));
}

void truesight(blessing)
int blessing;
{
  if (blessing > -1) {
    Player.status[TRUESIGHT]+=random_range(10)+1;
    mprint(LS(IDS_MSG_20874));
  }
  else {
    Player.status[BLINDED]+=random_range(10)+1;
    mprint(LS(IDS_MSG_20875));
  }
}



void dispel(blessing)
int blessing;     
{
  int i,x=Player.x,y=Player.y;
  if (blessing > -1) {
    setspot(&x,&y);
    if ((x==Player.x)&&(y==Player.y)) {
      for(i=0;i<MAXITEMS;i++) {
	if (Player.possessions[i]!=NULL)
	  if ((Player.possessions[i]->used) &&
	      (Player.possessions[i]->blessing < 0)) {
	    if (blessing+1 + Player.possessions[i]->blessing >=0) {
	      mprint(LS(IDS_MSG_20876));
	      mprint(itemid(Player.possessions[i]));
	      Player.possessions[i]->blessing = 0;
	    }
	    else {
	      mprint(LS(IDS_MSG_20877));
	      mprint(itemid(Player.possessions[i]));
	    }
	  }
      }
    }
    else if (Level->site[x][y].creature != NULL) {
      if (Level->site[x][y].creature->level < blessing * 3) {
	Level->site[x][y].creature->specialf = M_NO_OP;
	if (Level->site[x][y].creature->meleef != M_NO_OP)
	  Level->site[x][y].creature->meleef = M_MELEE_NORMAL;
	Level->site[x][y].creature->strikef = M_NO_OP;
	Level->site[x][y].creature->immunity=0;
	m_status_reset(Level->site[x][y].creature,M_INVISIBLE);	
	m_status_reset(Level->site[x][y].creature,INTANGIBLE);
      }
      else mprint(LS(IDS_MSG_20878));
    }
    else if ((Level->site[x][y].p_locf == L_TRAP_FIRE) ||
	     (Level->site[x][y].p_locf == L_STATUE_WAKE) ||
	     (Level->site[x][y].p_locf == L_TRAP_TELEPORT) ||
	     (Level->site[x][y].p_locf == L_TRAP_DISINTEGRATE)) {
      Level->site[x][y].p_locf = L_NO_OP;
      if (Level->site[x][y].locchar == TRAP)
	Level->site[x][y].locchar = FLOOR;
      lset(x, y, CHANGED);
    }
    else if (Level->site[x][y].p_locf == L_MAGIC_POOL)
      Level->site[x][y].p_locf = L_WATER;
    else mprint(LS(IDS_MSG_20879));
  }
  else {
    mprint(LS(IDS_MSG_20880));
    if (Player.status[ACCURACY] && (Player.status[ACCURACY] < 1000))
      Player.status[ACCURACY]=1;
    if (Player.status[DISPLACED]&&(Player.status[DISPLACED] < 1000))
	Player.status[DISPLACED]=1;
    if (Player.status[HASTED]&&(Player.status[HASTED] < 1000))
      Player.status[HASTED]=1;
    if (Player.status[BREATHING]&&(Player.status[BREATHING] < 1000))
      Player.status[BREATHING]=1;
    if (Player.status[INVISIBLE]&&(Player.status[INVISIBLE] < 1000))
	Player.status[INVISIBLE]=1;
    if (Player.status[REGENERATING]&&(Player.status[REGENERATING] < 1000))
      Player.status[REGENERATING]=1;
    if (Player.status[ALERT]&&(Player.status[ALERT] < 1000))
	Player.status[ALERT]=1;
    if (Player.status[HERO]&&(Player.status[HERO] < 1000))
	Player.status[HERO]=1;
    if (Player.status[LEVITATING]&&(Player.status[LEVITATING] < 1000))
	Player.status[LEVITATING]=1;
    if (Player.status[ACCURATE]&&(Player.status[ACCURATE] < 1000))
	Player.status[ACCURATE]=1;
    if (Player.status[TRUESIGHT]&&(Player.status[TRUESIGHT] < 1000))
	Player.status[TRUESIGHT]=1;
    tenminute_status_check();
  }
}


void polymorph(blessing)
int blessing;     
{
  int x=Player.x,y=Player.y,newmonster;
  struct monster *m;
  setspot(&x,&y);
  if ((x==Player.x)&&(y==Player.y)) {
    mprint(LS(IDS_MSG_20881));
    mprint(Monsters[random_range(NUMMONSTERS)].monstring);
    mprint(LS(IDS_MSG_20882));
    p_death("polymorphing oneself");
  }
  else if ((m=Level->site[x][y].creature) == NULL)
    mprint(LS(IDS_MSG_20883));
  else {
    if (m_immunityp(m,OTHER_MAGIC) || (m->level > random_range(12))) {
      strcpy(Str1,LS(IDS_MSG_23120));
      strcat(Str1,m->monstring);
      strcat(Str1,LS(IDS_MSG_23126));
      m_status_set(m,HOSTILE);
    }
    else {
      if (blessing < 0) {
	do newmonster = random_range(NUMMONSTERS);
	while ((newmonster == ML0+4) ||
	       (newmonster == ML7+3) ||
	       (Monsters[newmonster].level <= m->level) ||
	       (Monsters[newmonster].uniqueness != COMMON));
      }
      else {
	do newmonster = random_range(NUMMONSTERS);
	while ((newmonster == ML0+4) ||
	       (newmonster == ML7+3) ||
	       (Monsters[newmonster].uniqueness != COMMON));
      }
      m->id = Monsters[newmonster].id;
      m->hp = max(m->hp,Monsters[newmonster].id);
      m->speed = Monsters[newmonster].speed;
      m->hit = Monsters[newmonster].hit;
      m->ac = Monsters[newmonster].ac;
      m->dmg = Monsters[newmonster].dmg;
      m->sense = Monsters[newmonster].sense;
      m->wakeup = Monsters[newmonster].wakeup;
      m->level = max(m->level,Monsters[newmonster].level);
      m->status = Monsters[newmonster].status;
      m->immunity = (m->immunity | Monsters[newmonster].immunity);
      m->xpv = max(m->xpv,Monsters[newmonster].wakeup);
      m->transformid = Monsters[newmonster].transformid;
      m->corpsevalue = Monsters[newmonster].corpsevalue;
      m->corpseweight = Monsters[newmonster].corpseweight;
      m->monchar = Monsters[newmonster].monchar;
      m->meleestr = Monsters[newmonster].meleestr;
      m->monstring = Monsters[newmonster].monstring;
      m->corpsestr = Monsters[newmonster].corpsestr;
      m->talkf = Monsters[newmonster].talkf;
      m->movef = Monsters[newmonster].movef;
      m->meleef = Monsters[newmonster].meleef;
      m->strikef = Monsters[newmonster].strikef;
      m->specialf = Monsters[newmonster].specialf;
      m_status_set(m,HOSTILE);
    }
  }
}




void hellfire(x,y,blessing)
int x,y,blessing;
{
  struct monster *m;
  if ((x==Player.x)&&(y==Player.y)) {
    mprint(LS(IDS_MSG_20884));
    p_death("hellfire");
  }
  else if ((m=Level->site[x][y].creature) == NULL) {
    mprint(LS(IDS_MSG_20885));
    level_drain(5,"indiscriminate use of hellfire");
  }
  else {
    mprint(LS(IDS_MSG_20886));
    if (blessing < 0) {
      mprint(LS(IDS_MSG_20887));
      morewait();
      mprint(LS(IDS_MSG_20888));
      m->hp += 1000;
      m->hit +=20;
      m->dmg += 100;
      m_status_set(m,HOSTILE);
    }
    else {
      if (m->uniqueness == COMMON) {
	mprint(LS(IDS_MSG_20889));
	m->corpsestr = "a greasy spot";
	m->id = 0;
	free_objlist(m->possessions);
	m->possessions = NULL;
      }
      else
	mprint(LS(IDS_MSG_20890));
      m_death(m);
    }
  }
}


void drain(blessing)
int blessing;
{
  int x=Player.x,y=Player.y;
  struct monster *m;
  setspot(&x,&y);
  mprint(LS(IDS_MSG_20891));
  if ((x==Player.x)&&(y==Player.y)) {
    mprint(LS(IDS_MSG_20892));
    mprint(LS(IDS_MSG_20893));
    level_drain(Player.level,"self-vampirism");
  }
  else if ((m=Level->site[x][y].creature) != NULL) {
    if ((blessing > -1) && (! m_immunityp(m,NEGENERGY))) {
      mprint(LS(IDS_MSG_20894));
      m_damage(m,m->level*m->level,NEGENERGY);
      m->hit = max(m->hit - m->level, 1);
      m->dmg = max(m->dmg - m->level*m->level, 1);
      m->ac = max(m->ac - m->level, 1);
      m->level = max(1,m->level-1);
      mprint(LS(IDS_MSG_20895));
      gain_experience(m->level*5);
      Player.hp+=(m->level*m->level / 2);
    }
    else {
      mprint(LS(IDS_MSG_20896));
      mprint(LS(IDS_MSG_20897));
      m->hp+=Player.level*Player.level;
      m->hit += Player.level;
      m->dmg += Player.level*Player.level;
      m->ac += Player.level;
      m->level++;
      mprint(LS(IDS_MSG_20898));
      Player.mana = min(0,Player.level*Player.level);
      level_drain(m->level,"negative energy conflict");
    }
  }
  else if (blessing < 0) {
    mprint(LS(IDS_MSG_20899));
    level_drain(3,"reversed energy drain");
  }
  else if (Level->site[x][y].locchar == ALTAR) {
    mprint(LS(IDS_MSG_20900));
    Level->site[x][y].locchar = ABYSS;
    Level->site[x][y].p_locf = L_ABYSS;
    lset(x, y, CHANGED);
    if (! Player.patron) {
      mprint(LS(IDS_MSG_20901));
      gain_experience(40);
      Player.hp += 20;
      Player.pow+=2;
    }
    if (Level->site[x][y].aux == Player.patron) {
      mprint(LS(IDS_MSG_20902));
      mprint(LS(IDS_MSG_20903));
      p_damage(Player.hp-1,UNSTOPPABLE,"godsfire");
      mprint(LS(IDS_MSG_20904));
      Player.patron = -1;
      Player.rank[PRIESTHOOD] = 0;
    }
    else {
      mprint(LS(IDS_MSG_20905));
      p_damage(random_range(Player.level*10),UNSTOPPABLE,"divine wrath");
      if (Player.patron != 0) {
	mprint(LS(IDS_MSG_20906));
	gain_experience(100);
      }
    }
  }
  else {
    mprint(LS(IDS_MSG_20907));
    Player.hp++;
  }
}

void sanctuary()
{
  if (Level->environment == E_TEMPLE) 
    mprint(LS(IDS_MSG_20790));
  else {
    mprint(LS(IDS_MSG_20908));
    Player.sx = Player.x;
    Player.sy = Player.y;
  }
}

void shadowform()
{
  if (!Player.status[SHADOWFORM]) {
    mprint(LS(IDS_MSG_20909));
    Player.immunity[NORMAL_DAMAGE]+=Player.level;
    Player.immunity[ACID]+=Player.level;
    Player.immunity[THEFT]+=Player.level;
    Player.immunity[INFECTION]+=Player.level;
    Player.status[SHADOWFORM]+=Player.level;
  }
  else {
    mprint(LS(IDS_MSG_20910));
    Player.immunity[NORMAL_DAMAGE]++;
    Player.immunity[ACID]++;
    Player.immunity[THEFT]++;
    Player.immunity[INFECTION]++;
    Player.status[SHADOWFORM]++;
  }
}

void illuminate(blessing)
int blessing;
{
  int r=Level->site[Player.x][Player.y].roomnumber;
  if (blessing > -1) {
    if (r > ROOMBASE) {
      if (loc_statusp(Player.x,Player.y,LIT))
	mprint(LS(IDS_MSG_20911));
      else {
	mprint(LS(IDS_MSG_20912));
	Player.status[ILLUMINATION]+=blessing+3;
	spreadroomlight(Player.x,
			Player.y,
			Level->site[Player.x][Player.y].roomnumber);
      }
    }
    else mprint(LS(IDS_MSG_20913));
  }
  else {
    if (r > ROOMBASE) {
      if (! loc_statusp(Player.x,Player.y,LIT))
	mprint(LS(IDS_MSG_20778));
      else {
	mprint(LS(IDS_MSG_20914));
	spreadroomdark(Player.x,
		       Player.y,
		       Level->site[Player.x][Player.y].roomnumber);
      }
    }
    else mprint(LS(IDS_MSG_20915));
  }
}


void drain_life(amount)
int amount;
{
  amount = abs(amount);
  mprint(LS(IDS_MSG_20916));
  if (p_immune(NEGENERGY))
    mprint(LS(IDS_MSG_20917));
  else {
    if (random_range(2)) {
      mprint(LS(IDS_MSG_20918));
      Player.str-=amount;
      Player.con-=amount;
      if ((Player.str < 3) || (Player.con < 3)) {
	mprint(LS(IDS_MSG_20919));
	Player.hp = 0;
	strcpy(Str2,LS(IDS_MSG_23127));
	p_death(Str2);
      }
    }
    else {
      mprint(LS(IDS_MSG_20920));
      strcpy(Str2,LS(IDS_MSG_23128));
      level_drain(amount,Str2);
    }
  }
}


void inflict_fear(x,y)
int x,y;
{
  struct monster *m;
  if ((Player.x == x) && (Player.y == y)) {
    mprint(LS(IDS_MSG_20921));
    if (Player.immunity[FEAR] > 0)
      mprint(LS(IDS_MSG_20922));
    else {
      mprint(LS(IDS_MSG_20923));
      Player.status[AFRAID]+=10;
    }
  }
  else if ((m = Level->site[x][y].creature) != NULL) {
    if (m->uniqueness == COMMON) {
      strcpy(Str2,LS(IDS_MSG_23120));
      strcat(Str2,m->monstring);
    }
    else strcpy(Str2,m->monstring);
    m->speed = max(2,m->speed-1);
    if (m_immunityp(m,FEAR)) 
      strcat(Str2,LS(IDS_MSG_23129));
    else {
      strcat(Str2,LS(IDS_MSG_23130));
      m_dropstuff(m);
      if (m_statusp(m,MOBILE))
	m->movef = M_MOVE_SCAREDY;
    }
  }
  else mprint(LS(IDS_MSG_20924));
}




/*Turns on deflection status for the player */
void deflection(blessing)
int blessing;
{
  if (blessing > -1) {
      mprint(LS(IDS_MSG_20925));
      Player.status[DEFLECTION] = blessing + random_range(6);
    }
  else {
    mprint(LS(IDS_MSG_20768));
    Player.status[VULNERABLE] += random_range(6) - blessing;
  }
}
