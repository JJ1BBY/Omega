/* omega copyright (C) 1987,1988,1989 by Laurence Raphael Brothers */
/* itemf1.c */

/* various item functions: potions,scrolls,boots,cloaks,things,food */

#include "glob.h"


/* general item functions */

void i_no_op(o)
pob o;
{
}

void i_nothing(o)
pob o;
{
}





/*  scroll functions */

void i_knowledge(o)
pob o;
{
  if (o->blessing > -1)
    Objects[o->id].known = 1;
  knowledge(o->blessing);
}

void i_jane_t(o)
pob o;
{
  int volume = random_range(6);
  int i,j,k;
  char v;

  print1(LS(IDS_MSG_21363));
  
  switch(volume) {
    case 0:nprint1(LS(IDS_MSG_21364));j = SCROLLID; k = POTIONID; break;
    case 1:nprint1(LS(IDS_MSG_21365));j = POTIONID; k = WEAPONID; break;
    case 2:nprint1(LS(IDS_MSG_21366));j = CLOAKID; k = BOOTID; break;
    case 3:nprint1(LS(IDS_MSG_21367));j = BOOTID; k = RINGID; break;
    case 4:nprint1(LS(IDS_MSG_21368));j = RINGID; k = STICKID; break;
    case 5:nprint1(LS(IDS_MSG_21369));j = STICKID; k = ARTIFACTID; break;
  }
  menuclear();
  menuprint("You could probably now recognise:\n");
  for(i=j;i<k;i++) {
    Objects[i].known = 1;
    v = Objects[i].truename[0];
    if ((v >= 'A' && v <= 'Z') || volume == 3)
      sprintf(Str1, "   %s\n", Objects[i].truename);
    else if (v == 'a' || v == 'e' || v == 'i' || v == 'o' || v == 'u')
      sprintf(Str1, "   an %s\n", Objects[i].truename);
    else
      sprintf(Str1, "   a %s\n", Objects[i].truename);
    menuprint(Str1);
  }
  showmenu();
  morewait();
  xredraw();
}

   
void i_flux(o)
pob o;
{
  if (o->blessing > -1)
    Objects[o->id].known = 1;
  flux(o->blessing);
}


/* enchantment */
void i_enchant(o)
pob o;
{
  if (o->blessing > -1)
    Objects[o->id].known = 1;
  enchant(o->blessing < 0 ? -1-o->plus : o->plus+1);
}

/* scroll of clairvoyance */
void i_clairvoyance(o)
struct object *o;
{
  if (o->blessing > -1)
    Objects[o->id].known = 1;
  if (o->blessing < 0)
    amnesia();
  else clairvoyance(5+o->blessing*5);
}



void i_acquire(o)
pob o;
{
  int blessing;

  if (o->blessing > -1)
    Objects[o->id].known = 1;
  blessing = o->blessing;
  *o = Objects[SCROLLID+0]; /* blank out the scroll */
  acquire(blessing);
}

void i_teleport(o)
pob o;
{
  if (o->blessing > -1)
    Objects[o->id].known = 1;
  p_teleport(o->blessing);
}


void i_spells(o)
pob o;
{
  if (o->blessing > -1)
    Objects[o->id].known = 1;
  mprint(LS(IDS_MSG_21370));
  morewait();
  learnspell(o->blessing);
}



/* scroll of blessing */
void i_bless(o)
pob o;
{
  Objects[o->id].known = 1;
  bless(o->blessing);
}

/* scroll of wishing */
void i_wish(o)
pob o;
{
  if (o->blessing > -1)
    Objects[o->id].known = 1;
  wish(o->blessing);
  *o = Objects[SCROLLID+0]; /* blank out the scroll */
}

/* scroll of displacement */
void i_displace(o)
pob o;
{
  if (o->blessing > -1) 
      Objects[o->id].known = 1;
  displace(o->blessing);
}


/* scroll of deflection */
void i_deflect(o)
pob o;
{
  if (o->blessing > -1) 
    Objects[o->id].known = 1;
  deflection(o->blessing);
}

/* scroll of identification */
void i_id(o)
pob o;
{
  if (o->blessing > -1)
    Objects[o->id].known = 1;
  identify(o->blessing);
}

/* potion functions */

/* potion of healing */
void i_heal(o)
pob o;
{
  if (o->blessing > -1) {
    Objects[o->id].known = 1;
    heal(1+o->plus);
  }
  else heal(-1-abs(o->plus));
}

/* potion of monster detection */
void i_mondet(o)
pob o;
{
  if (o->blessing > -1)
    Objects[o->id].known = 1;
  mondet(o->blessing);
}


/* potion of object detection */
void i_objdet(o)
pob o;
{

  if (o->blessing > -1)
    Objects[o->id].known = 1;
  objdet(o->blessing);
}

/* potion of neutralize poison */
void i_neutralize_poison(o)
pob o;
{
  if (o->blessing > -1) {
    Objects[o->id].known = 1;
    mprint(LS(IDS_MSG_21371));
    Player.status[POISONED] = 0;
  }
  else p_poison(random_range(20)+5);
}


/* potion of sleep */
void i_sleep_self(o)
pob o;
{
  sleep_player(6);
  Objects[o->id].known = 1;
}




/* potion of speed */
void i_speed(o)
pob o;
{
  if (o->blessing > -1) 
    Objects[o->id].known = 1;
  haste(o->blessing);
}


/* potion of restoration */
void i_restore(o)
pob o;
{
  if (o->blessing > -1) 
    Objects[o->id].known = 1;
  recover_stat(o->blessing);
}

void i_augment(o)
pob o;
{
  if (o->blessing > -1) 
    Objects[o->id].known = 1;
  augment(o->blessing);
}  

void i_azoth(o)
pob o;
{
  if (o->plus < 0) {
    mprint(LS(IDS_MSG_21372));
    p_poison(25);
  }
  else if (o->plus == 0) {
    mprint(LS(IDS_MSG_21373));
    Player.con = ((int) (Player.con / 2));
    calc_melee();
  }
  else if (o->blessing < 1) {
    mprint(LS(IDS_MSG_21374));
    Player.pow = Player.maxpow = ((int) (Player.maxpow / 2));
    level_drain(random_range(10),"cursed azoth");
  }
  else {
    mprint(LS(IDS_MSG_21375));
    if (Player.str > Player.maxstr*2) {
      mprint(LS(IDS_MSG_21376));
      p_death("overdose of azoth");
    }
    else {
      heal(10);
      cleanse(1);
      Player.mana = calcmana()*3;
      Player.str = (Player.maxstr++)*3;
    }
  }
}

void i_regenerate(o)
pob o;
{
  regenerate(o->blessing);
}





/* boots functions */
void i_perm_speed(o)
pob o;
{
  if (o->blessing > -1) {
    if (o->used) {
      o->known = 2;
      Objects[o->id].known = 1;
      if (Player.status[SLOWED] > 0) {
	Player.status[SLOWED] = 0;
      }
      mprint(LS(IDS_MSG_20777)); 
      Player.status[HASTED] += 1500;
    }
    else {
      Player.status[HASTED] -= 1500;
      if (Player.status[HASTED] < 1) 
	mprint(LS(IDS_MSG_21377));
    }
  }
  else {
    if (o->used) {
      if (Player.status[HASTED] > 0) {
	Player.status[HASTED] = 0;
      }
      mprint(LS(IDS_MSG_20779));
      Player.status[SLOWED] += 1500;
    }
    else  {
      Player.status[SLOWED] -= 1500;
      if (Player.status[SLOWED] < 1) 
	mprint(LS(IDS_MSG_21378));
    }  
  }
}

/* cloak functions */
void i_perm_displace(o)
pob o;
{
  if (o->blessing > -1) {
    if (o->used) {
      mprint(LS(IDS_MSG_21379));
      Player.status[DISPLACED] += 1500;
    }
    else {
      Player.status[DISPLACED] -= 1500;
      if (Player.status[DISPLACED] < 1) {
	mprint(LS(IDS_MSG_20115));
	Player.status[DISPLACED] = 0;
      }
    }
  }
  else {
    if (o->used) {
      mprint(LS(IDS_MSG_21380));
      Player.status[VULNERABLE] += 1500;
    }
    else {
      Player.status[VULNERABLE] -= 1500;
      if (Player.status[VULNERABLE] < 1) {
	mprint(LS(IDS_MSG_20104));
	Player.status[VULNERABLE] = 0;
      }
    }
  }
}

void i_perm_negimmune(o)
pob o;
{
  if (o->blessing > -1) {
    if (o->used) {
      Player.immunity[NEGENERGY]++;
    }
    else Player.immunity[NEGENERGY]--;
  }
  else if (o->used)
    level_drain(abs(o->blessing),"cursed cloak of level drain");
}

/* food functions */


void i_food(o)
pob o;
{
  switch(random_range(5)) {
    case 0: mprint(LS(IDS_MSG_21381)); break;
    case 1: mprint(LS(IDS_MSG_21382)); break;
    case 2: mprint(LS(IDS_MSG_21383)); break;
    case 3: mprint(LS(IDS_MSG_21384)); break;
    case 4: mprint(LS(IDS_MSG_21385)); break;
  }
}

void i_stim(o)
pob o;
{
  mprint(LS(IDS_MSG_21386));
  i_speed(o);
  Player.str +=3;
  Player.con -=1;
  calc_melee();
}

void i_pow(o)
pob o;
{
  mprint(LS(IDS_MSG_21387));
  Player.mana = 2 * calcmana();
}

void i_poison_food(o)
pob o;
{
  mprint(LS(IDS_MSG_21388));
  p_poison(random_range(20)+5);
}

void i_pepper_food(o)
pob o;
{
  mprint(LS(IDS_MSG_21389));
  morewait();
  mprint(LS(IDS_MSG_21390));
  morewait();
  mprint(LS(IDS_MSG_21391));
  morewait();
  mprint(LS(IDS_MSG_21392));
  morewait();
  mprint(LS(IDS_MSG_21393));
  morewait();
  p_damage(1,UNSTOPPABLE,"a szechuan pepper");
  mprint(LS(IDS_MSG_21394));
  mprint(LS(IDS_MSG_21395));
  mprint(LS(IDS_MSG_21396));
  Player.immunity[SLEEP]++;
}

void i_lembas(o)
pob o;
{
  heal(10);
  cleanse(0);
  Player.food = 40;
}


void i_cure(o)
pob o;
{
  cure(o->blessing);
}

void i_immune(o)
pob o;
{
  if (o->blessing > 0) {
    mprint(LS(IDS_MSG_21397));
    Player.immunity[INFECTION]++;
    cure(o->blessing);
  }
}



void i_breathing(o)
pob o;
{
  
  if (o->blessing > -1)
    Objects[o->id].known = 1;
  breathe(o->blessing);
}

      
void i_invisible(o)
pob o;
{
  if (o->blessing > -1)
    Objects[o->id].known = 1;
  invisible(o->blessing);
}


void i_perm_invisible(o)
pob o;
{
  if (o->blessing > -1)
    Objects[o->id].known = 1;
  if (o->used) {
    if (o->blessing > -1) {
      mprint(LS(IDS_MSG_20769));
      Player.status[INVISIBLE] += 1500;
    }
    else {
      mprint(LS(IDS_MSG_21398));
      Player.status[VULNERABLE] += 1500;
    }
  }
  else {
    if (o->blessing > -1) {
      Player.status[INVISIBLE]-=1500;
      if (Player.status[INVISIBLE] < 1) {
	mprint(LS(IDS_MSG_21399));
	Player.status[INVISIBLE] = 0;
      }
    }
    else {
      Player.status[VULNERABLE] -= 1500;
      if (Player.status[VULNERABLE] < 1) {
	mprint(LS(IDS_MSG_21400));
	Player.status[VULNERABLE] = 0;
      }
    }
  }
}


void i_warp(o)
pob o;
{
  if (o->blessing > -1)
    Objects[o->id].known = 1;
  warp(o->blessing);
}


void i_alert(o)
pob o;
{
  if (o->blessing > -1) {
    Objects[o->id].known = 1;
    alert(o->blessing);
  }
}

void i_charge(o)
pob o;
{
  int i;
  if (o->blessing > -1)
    Objects[o->id].known = 1;
  mprint(LS(IDS_MSG_21401));
  mprint(LS(IDS_MSG_21402));
  i = getitem(STICK);
  if (i != ABORT) {
    if (o->blessing < 0) {
      mprint(LS(IDS_MSG_21403));
      Player.possessions[i]->charge = 0;
    }
    else {
      mprint(LS(IDS_MSG_21404));
      Player.possessions[i]->charge += (random_range(10)+1)*(o->blessing+1);
      if (Player.possessions[i]->charge > 99)
	Player.possessions[i]->charge = 99;
    }
  }
}


void i_fear_resist(o)
pob o;
{
  if (o->blessing > -1) {
    Objects[o->id].known = 1;
    if (Player.status[AFRAID] > 0) {
      mprint(LS(IDS_MSG_21405));
      Player.status[AFRAID] = 0;
    }
  }
  else if (! p_immune(FEAR)) {
    mprint(LS(IDS_MSG_20923));
    Player.status[AFRAID]+=random_range(100);
  }
}



/* use a thieves pick */
void i_pick(o)
pob o;
{
  int dir;
  int ox,oy;
  o->used = FALSE;
  if ((! o->known) && (! Player.rank[THIEVES]))
    mprint(LS(IDS_MSG_21406));
  else {
    o->known = 1;
    Objects[o->id].known = 1;
    mprint(LS(IDS_MSG_21407));
    dir = getdir();
    if (dir == ABORT)
      resetgamestatus(SKIP_MONSTERS);
    else {
      ox = Player.x + Dirs[0][dir];
      oy = Player.y + Dirs[1][dir];
      if ((Level->site[ox][oy].locchar != CLOSED_DOOR) || 
	  loc_statusp(ox,oy,SECRET)) {
	mprint(LS(IDS_MSG_21408));
	resetgamestatus(SKIP_MONSTERS);
      }
      else if (Level->site[ox][oy].aux == LOCKED) {
	if (Level->depth == MaxDungeonLevels-1) 
	  mprint(LS(IDS_MSG_21409));
	else if (Level->depth*2 + random_range(50) <
	    Player.dex+Player.level+Player.rank[THIEVES]*10) {
	  mprint(LS(IDS_MSG_21410));
	  Level->site[ox][oy].aux = UNLOCKED;
	  lset(ox, oy, CHANGED);
	  gain_experience(max(3,Level->depth));
	}
	else mprint(LS(IDS_MSG_21411));
      }
      else mprint(LS(IDS_MSG_21412));
    }
  }
}

/* use a magic key*/
void i_key(o)
pob o;
{
  int dir;
  int ox,oy;
  o->used = FALSE;
  mprint(LS(IDS_MSG_21413));
  dir = getdir();
  if (dir == ABORT)
    resetgamestatus(SKIP_MONSTERS);
  else {
    ox = Player.x + Dirs[0][dir];
    oy = Player.y + Dirs[1][dir];
    if ((Level->site[ox][oy].locchar != CLOSED_DOOR) || 
	loc_statusp(ox,oy,SECRET)) {
	  mprint(LS(IDS_MSG_21408));
	  resetgamestatus(SKIP_MONSTERS);
	}
    else if (Level->site[ox][oy].aux == LOCKED) {
	mprint(LS(IDS_MSG_21414));
	Level->site[ox][oy].aux = UNLOCKED;
	lset(ox, oy, CHANGED);
	o->blessing--;
	if ((o->blessing<0)||(Level->depth == MaxDungeonLevels-1)) {
	  mprint(LS(IDS_MSG_21415));
	  conform_lost_objects(1,o);
	}
	else
	  mprint(LS(IDS_MSG_21416));
      }
    else mprint(LS(IDS_MSG_21412));
  }
}

void i_corpse(o)
pob o;
{
  switch (o->aux) {
  case ML0+1: 
  case ML0+2:
  case ML0+3:
  case ML0+4:
  case ML2+0:
  case ML2+2:
  case ML7+3:
  case ML10+0: /* cannibalism */
    mprint(LS(IDS_MSG_21417));
    if (Player.alignment > 0) Player.food = 25;
    Player.food += 8;
    Player.alignment -=10;
    foodcheck();
    break;
  case ML1+2: /* fnord */
    mprint(LS(IDS_MSG_21418));
    Player.iq++;
    break;
  case ML4+3: /* denebian slime devil */
    mprint(LS(IDS_MSG_21419));
    mprint(LS(IDS_MSG_21420));
    break;
  case ML5+0:
    mprint(LS(IDS_MSG_21421));
    Player.food=24;
    foodcheck();
    break;
  case ML7+0:
    mprint(LS(IDS_MSG_21422));
    Player.str = max(Player.str,Player.maxstr+10);
    Player.food = 24;
    foodcheck();
    break;
  case ML9+1:
    mprint(LS(IDS_MSG_21423));
    if (Player.status[INVISIBLE] < 1000) Player.status[INVISIBLE] = 666;
    Player.food+=6;
    foodcheck();
    break;
  case ML7+2:  
    mprint(LS(IDS_MSG_21424));
    Player.immunity[POISON]=1000;
    break;
  case ML0+0:
  case ML1+0:
  case ML1+1:
  case ML1+4:
  case ML1+5:
  case ML1+6:
  case ML1+10:
  case ML2+1:
  case ML2+4:
  case ML4+1:
  case ML4+4:
  case ML5+3:
    mprint(LS(IDS_MSG_21425));
    Player.food++;
    foodcheck();
    break;
  case ML1+3:
  case ML1+7:
  case ML2+3:
  case ML2+5:
  case ML3+1:
  case ML4+5:
  case ML9+3:
  case ML10+1:
    mprint(LS(IDS_MSG_21426));
    mprint(LS(IDS_MSG_21427));
    Player.food = min(Player.food, 4);
    if (! Player.immunity[INFECTION])
      Player.status[DISEASED]+=24;
    p_poison(10);
    break;
  default: mprint(LS(IDS_MSG_21428));
  }
}

void i_accuracy(o)
pob o;
{
  o->known = 1;
  Objects[o->id].known = 1;
  accuracy(o->blessing);
}

void i_perm_accuracy(o)
pob o;
{
  o->known = 1;
  Objects[o->id].known = 1;
  if ((o->used) && (o->blessing > -1)) {
    Player.status[ACCURATE] += 1500;
    mprint(LS(IDS_MSG_21429));
  }
  else {
    Player.status[ACCURATE] -= 1500;
    if (Player.status[ACCURATE] < 1) {
      Player.status[ACCURATE] = 0;
      calc_melee();
      mprint(LS(IDS_MSG_21430));
    }
  }
}

void i_hero(o)
pob o;
{
  o->known = 1;
  Objects[o->id].known = 1;
  hero(o->blessing);
}

void i_perm_hero(o)
pob o;
{
  o->known = 1;
  Objects[o->id].known = 1;
  if (o->used){
    if (o->blessing > -1) {
      Player.status[HERO] += 1500;
      calc_melee();
      mprint(LS(IDS_MSG_20858));
    }
    else {
      Player.status[HERO] = 0;
      calc_melee();
      if (! Player.immunity[FEAR]) {
	Player.status[AFRAID]+=1500;
	mprint(LS(IDS_MSG_21431));
      }
    }
  }
  else {
    if (o->blessing > -1) {
      Player.status[HERO] -= 1500;
      if (Player.status[HERO] < 1) {
	calc_melee();
	mprint(LS(IDS_MSG_21432));
	Player.status[HERO] = 0;
      }
    }
    else {
      Player.status[AFRAID] -= 1500;
      if (Player.status[AFRAID] < 1) {
	mprint(LS(IDS_MSG_21433));
	Player.status[AFRAID] = 0;
      }
    }
  } 
}

void i_levitate(o)
pob o;
{
  o->known = 1;
  Objects[o->id].known = 1;
  levitate(o->blessing);
}

void i_perm_levitate(o)
pob o;
{
  o->known = 1;
  Objects[o->id].known = 1;
  if (o->blessing > -1) {
    if (o->used) {
      Player.status[LEVITATING] += 1400;
      mprint(LS(IDS_MSG_21434));
      mprint(LS(IDS_MSG_21435));
    }
    else {
      Player.status[LEVITATING] -= 1500;
      if (Player.status[LEVITATING] < 1) {
	Player.status[LEVITATING] = 0;
	mprint(LS(IDS_MSG_21436));
      }
    }
  }
  else i_perm_burden(o);
}

void i_perm_protection(o)
pob o;
{
  if (o->used){
    if (o->blessing > -1)
      Player.status[PROTECTION] += abs(o->plus)+1;
    else
      Player.status[PROTECTION] -= abs(o->plus)+1;
  }
  else {
    if (o->blessing > -1)
      Player.status[PROTECTION] -= abs(o->plus)+1;
    else
      Player.status[PROTECTION] += abs(o->plus)+1;
  }
  calc_melee();
}

void i_perm_agility(o)
pob o;
{
  o->known = 2;
  Objects[o->id].known = 1;
  if (o->used){
    if (o->blessing > -1)
      Player.agi += abs(o->plus)+1;
    else
      Player.agi -= abs(o->plus)+1;
  }
  else {
    if (o->blessing > -1)
      Player.agi -= abs(o->plus)+1;
    else
      Player.agi += abs(o->plus)+1;
  }
  calc_melee();
}

void i_truesight(o)
pob o;
{
  o->known = 1;
  Objects[o->id].known = 1;
  truesight(o->blessing);
}

void i_perm_truesight(o)
pob o;
{
  o->known = 1;
  Objects[o->id].known = 1;
  if (o->used){
    if (o->blessing > -1) {
      Player.status[TRUESIGHT] += 1500;
      mprint(LS(IDS_MSG_21437));
    }
    else {
      Player.status[BLINDED] += 1500;
      mprint(LS(IDS_MSG_20875));
    }
  }
  else {
    if (o->blessing > -1) {
      Player.status[TRUESIGHT] -= 1500;
      if (Player.status[TRUESIGHT] < 1) {
	mprint(LS(IDS_MSG_20111));
	Player.status[TRUESIGHT] = 0;
      }
    }
    else {
      Player.status[BLINDED] -= 1500;
      if (Player.status[BLINDED] < 1) {
	mprint(LS(IDS_MSG_21438)); 
	Player.status[BLINDED] = 0;
      }
    }
  } 
}

void i_illuminate(o)
pob o;
{
  o->known = 1;
  Objects[o->id].known = 1;
  illuminate(o->blessing);
}

void i_perm_illuminate(o)
pob o;
{
  o->known = 1;
  Objects[o->id].known = 1;
  if (o->used) 
    Player.status[ILLUMINATION]+=1500;
  else
    Player.status[ILLUMINATION]=max(0,Player.status[ILLUMINATION]-1500);
}




void i_trap(o)
pob o;
{
  Objects[o->id].known = 1;
  
  if ((Level->site[Player.x][Player.y].locchar != FLOOR) ||
      (Level->site[Player.x][Player.y].p_locf != L_NO_OP))
    mprint(LS(IDS_MSG_21439));
  else  if (! o->known) {
    mprint(LS(IDS_MSG_21440));
    p_movefunction(o->aux);
  }
  else {
    mprint(LS(IDS_MSG_21441));
    Level->site[Player.x][Player.y].p_locf = o->aux;
    lset(Player.x, Player.y, CHANGED);
  }
  dispose_lost_objects(1,o);
}


void i_raise_portcullis(o)
pob o;
{
  l_raise_portcullis();
  mprint(LS(IDS_MSG_21442));
  conform_lost_objects(1,o);
}

