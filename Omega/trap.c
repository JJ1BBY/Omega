/* omega copyright (C) by Laurence Raphael Brothers, 1987,1988,1989 */
/* trap.c */
/* trap movement functions */

#include "glob.h"


/* various traps */

/* note special function on different levels */
void l_trap_siren()
{
  pml ml;
  Level->site[Player.x][Player.y].locchar = TRAP;
  lset(Player.x, Player.y, CHANGED);
  print1(LS(IDS_MSG_22609));
  print2(LS(IDS_MSG_22610));
  print3(LS(IDS_MSG_22611));
  morewait();
  clearmsg();
  if ((Current_Environment == E_HOUSE) ||
      (Current_Environment == E_MANSION)) {
    if (! gamestatusp(DESTROYED_ORDER)) {
      print1(LS(IDS_MSG_22612));
      print2(LS(IDS_MSG_22613));
      morewait();
      send_to_jail();
    }
    else print1(LS(IDS_MSG_22614));
  }
  else if (Current_Environment == E_HOVEL) 
    print1(LS(IDS_MSG_22614));
  else {
    if (Current_Environment == E_CIRCLE) {
      summon(-1,ML9+7); /* prime circle demon */
      summon(-1,ML9+7);
      summon(-1,ML9+7);
    }
    for(ml=Level->mlist;ml!=NULL;ml=ml->next) {
      m_status_set(ml->m,AWAKE);
      ml->m->sense *= 2;
      if ((Current_Environment == E_CIRCLE) ||
	  ((Current_Environment == E_VILLAGE) && (ml->m->id == ML0+3)) ||
	  ((Current_Environment == E_CITY) && (ml->m->id == ML0+3)))
	m_status_set(ml->m,HOSTILE);
    }
  }
}

void l_trap_dart()
{
  Level->site[Player.x][Player.y].locchar = TRAP;
  lset(Player.x, Player.y, CHANGED);
  if (gamestatusp(MOUNTED)) {
    mprint(LS(IDS_MSG_22615));
  }
  else if (random_range(100) < Player.absorption) 
    mprint(LS(IDS_MSG_22616));
  else {
    mprint(LS(IDS_MSG_22617));
    p_damage(difficulty(),NORMAL_DAMAGE,"a dart trap");
    if (random_range(3)==1) {
      mprint(LS(IDS_MSG_22618));
      morewait();
      p_poison(difficulty());
    }
  }
}

void l_trap_pit()
{
  Level->site[Player.x][Player.y].locchar = TRAP;
  lset(Player.x, Player.y, CHANGED);
  if (gamestatusp(MOUNTED)) {
    mprint(LS(IDS_MSG_22619));
    morewait();
    mprint(LS(IDS_MSG_22620));
    morewait();
    resetgamestatus(MOUNTED);
    showflags();
  }
  else if (Player.itemweight < ((int) (Player.maxweight / 2)))
    mprint(LS(IDS_MSG_22621));
  else {
    mprint(LS(IDS_MSG_22622));
    if (random_range(3) == 1) {
      mprint(LS(IDS_MSG_22623));
      morewait();
      p_damage(difficulty()*5,NORMAL_DAMAGE,"a spiked pit");
    }
    else p_damage(difficulty()*2,NORMAL_DAMAGE,"a pit");
    Player.status[IMMOBILE] ++;
  }
}

void l_trap_door()
{
  if (Current_Environment != Current_Dungeon)
    mprint(LS(IDS_MSG_22624));
  else {
    Level->site[Player.x][Player.y].locchar = TRAP;
    lset(Player.x, Player.y, CHANGED);
    if (gamestatusp(MOUNTED)) {
      mprint(LS(IDS_MSG_22625));
      morewait();
      mprint(LS(IDS_MSG_22626));
      morewait();
      if (Level->site[Player.x][Player.y].aux != S_DISINTEGRATE) {
	mprint(LS(IDS_MSG_22627));
	morewait();
	p_damage(Level->depth*difficulty(),UNSTOPPABLE,"a rockslide");
      }
      change_level(Level->depth,Level->depth+1,FALSE);
      resetgamestatus(MOUNTED);
      showflags();
      roomcheck();
    }
    else if (random_range(100) < Player.agi)
      mprint(LS(IDS_MSG_22628));
    else {
      mprint(LS(IDS_MSG_22629));
      morewait();
      p_damage(difficulty(),NORMAL_DAMAGE,"a trap door");
      if (Level->site[Player.x][Player.y].aux != S_DISINTEGRATE) {
	mprint(LS(IDS_MSG_22627));
	morewait();
	p_damage(Level->depth*difficulty(),UNSTOPPABLE,"a rockslide");
      }
      change_level(Level->depth,Level->depth+1,FALSE);
      roomcheck();
    }
  }
}

void l_trap_snare()
{
  Level->site[Player.x][Player.y].locchar = TRAP;
  lset(Player.x, Player.y, CHANGED);
  if (gamestatusp(MOUNTED)) 
    mprint(LS(IDS_MSG_22630));
  else if (random_range(100) < Player.agi)
    mprint(LS(IDS_MSG_22631));
  else {
    mprint(LS(IDS_MSG_22632));
    Player.status[IMMOBILE]+=difficulty();
  }
}

void l_trap_blade()
{
  Level->site[Player.x][Player.y].locchar = TRAP;
  lset(Player.x, Player.y, CHANGED);
  if (random_range(30) < Player.agi+Player.level)
    mprint(LS(IDS_MSG_22633));
  else {
    if (gamestatusp(MOUNTED)) {
      mprint(LS(IDS_MSG_22634));
      morewait();
      resetgamestatus(MOUNTED);
      showflags();
    }
    else {
      mprint(LS(IDS_MSG_22635));
      morewait();
      p_damage(random_range(difficulty()*3)+difficulty()-Player.absorption,
	       NORMAL_DAMAGE,
	       "a blade trap");
    }
  }
}

void l_trap_fire()
{
  if (gamestatusp(MOUNTED)) {
    mprint(LS(IDS_MSG_22636));
    morewait();
    resetgamestatus(MOUNTED);
    showflags();
  }
  else if (random_range(50) < Player.agi+Player.level)
    mprint(LS(IDS_MSG_22637));
  else {
    mprint(LS(IDS_MSG_22638));
    morewait();
    p_damage(random_range((difficulty()+1)*5),FLAME,"a fire trap");
  }
  Level->site[Player.x][Player.y].locchar = FIRE;
  Level->site[Player.x][Player.y].p_locf = L_FIRE;
  lset(Player.x, Player.y, CHANGED);
}

void l_trap_teleport()
{
  Level->site[Player.x][Player.y].locchar = TRAP;
  lset(Player.x, Player.y, CHANGED);
  mprint(LS(IDS_MSG_22639));
  morewait();
  if (random_range(10000) > difficulty()*difficulty()) p_teleport(0);
  else p_teleport(-1);
}

void l_trap_disintegrate()
{
  Level->site[Player.x][Player.y].locchar = TRAP;
  lset(Player.x, Player.y, CHANGED);
  mprint(LS(IDS_MSG_22640));
  morewait();
  if (gamestatusp(MOUNTED)) {
    mprint(LS(IDS_MSG_22641));
    morewait();
    resetgamestatus(MOUNTED);
    showflags();
  }
  disintegrate(Player.x,Player.y);
}

void l_trap_sleepgas()
{
  Level->site[Player.x][Player.y].locchar = TRAP;
  lset(Player.x, Player.y, CHANGED);
  mprint(LS(IDS_MSG_22642));
  morewait();
  mprint(LS(IDS_MSG_22643));
  morewait();
  sleep_player((difficulty()/5)+1);
}


void l_trap_acid()
{
  int i,k,itemdamage;
  Level->site[Player.x][Player.y].locchar = TRAP;
  lset(Player.x, Player.y, CHANGED);
  if (Player.agi+Player.level < random_range(100)) {
    mprint(LS(IDS_MSG_22644));
    morewait();
    if (gamestatusp(MOUNTED)) {
      mprint(LS(IDS_MSG_22645));
      morewait();
      resetgamestatus(MOUNTED);
      showflags();
    }
    p_damage(random_range(difficulty()*5),ACID,"an acid trap");
    if (! p_immune(ACID)) {
      mprint(LS(IDS_MSG_22646));
      morewait();
      itemdamage = random_range(5);
      for(i=k=0;((i<MAXITEMS) && (k<itemdamage));i++)
	if (Player.possessions[i]!=NULL) {
	  k++;
	  (void) damage_item(Player.possessions[i]);
	}
    }
  }
  else mprint(LS(IDS_MSG_22647));
}

void l_trap_abyss()
{
  Level->site[Player.x][Player.y].locchar = ABYSS;
  lset(Player.x, Player.y, CHANGED);
  if (gamestatusp(MOUNTED)) {
    mprint(LS(IDS_MSG_22648));
    morewait();
    l_abyss();
  }
  if (Player.dex + Player.level < random_range(100)) {
    mprint(LS(IDS_MSG_22649));
    morewait();
    l_abyss();
  }
  else mprint(LS(IDS_MSG_22650));
}

void l_trap_manadrain()
{
  Level->site[Player.x][Player.y].locchar = TRAP;
  lset(Player.x, Player.y, CHANGED);
  if (Player.mana==0) {
    if (random_range(10)==3) {
      mprint(LS(IDS_MSG_22651));
      mprint(LS(IDS_MSG_22652));
      morewait();
      Level->site[Player.x][Player.y].locchar = FLOOR;
      Level->site[Player.x][Player.y].p_locf = L_NO_OP;
      lset(Player.x, Player.y, CHANGED);
      Player.mana = calcmana();
    }
    else mprint(LS(IDS_MSG_22653));
  }
  else {
    mprint(LS(IDS_MSG_22654));
    mprint(LS(IDS_MSG_22655));
    morewait();
    while (Player.mana > 1) {
      Player.mana /= 2;
      dataprint();
    }
    Player.mana = 0;
    dispel(-1);
  }
}
