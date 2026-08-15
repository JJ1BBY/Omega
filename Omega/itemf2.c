/* omega copyright (C) 1987,1988,1989 by Laurence Raphael Brothers */
/* itemf2.c */

/* mostly ring, armor, and weapon functions */

#include "glob.h"




/* ring functions */
void i_perm_knowledge(o)
pob o;
{
  if (o->known < 1)
    o->known = 1;
  if (o->blessing > -1)
    Objects[o->id].known = 1;
  if (o->used) 
    knowledge(o->blessing);
}

void i_perm_strength(o)
pob o;
{
  if (o->known < 1) o->known = 1;
  Objects[o->id].known = 1;
  if (o->used){
    if (o->blessing > -1)
      Player.str += abs(o->plus)+1;
    else
      Player.str -= abs(o->plus)+1;
  }
  else {
    if (o->blessing > -1)
      Player.str -= abs(o->plus)+1;
    else
      Player.str += abs(o->plus)+1;
  }
  calc_melee();
}



void i_perm_burden(o)
pob o;
{
  int i;
  
  if (o->used) {
    o->weight = 1000;
    mprint(LS(IDS_MSG_21443));
  }
  else {
    o->weight = 1;
    mprint(LS(IDS_MSG_21444));
  }
  Player.itemweight = 0;
  for (i=0;i<MAXITEMS;i++) {
    if (Player.possessions[i] != NULL)
      Player.itemweight += 
	(Player.possessions[i]->weight*Player.possessions[i]->number);
  }
}

void i_perm_gaze_immune(o)
pob o;
{
  if (o->used) Player.immunity[GAZE]++;
  else Player.immunity[GAZE]--;
}

void i_perm_fire_resist(o)
pob o;
{
  if (o->used) Player.immunity[FLAME]++;
  else Player.immunity[FLAME]--;
}

void i_perm_poison_resist(o)
pob o;
{
  if (o->used) {
    if (o->blessing < 0) {
      Player.immunity[POISON] = 0;
      p_poison(100);
    }
    else {
      Player.immunity[POISON]++;
      if (Player.status[POISONED] > 0) {
	mprint(LS(IDS_MSG_21445));
	Player.status[POISONED] = 0;
      }
    }
  }
  else {
    Player.status[POISONED] = 0;
    Player.immunity[POISON]--;
  }
}






void i_perm_regenerate(o)
pob o;
{
  if (o->known < 1)  o->known = 1;
  if (o->blessing > -1)
    Objects[o->id].known = 1;
  if (o->used) {
    mprint(LS(IDS_MSG_21446));
    Player.status[REGENERATING] += 1500;
  }
  else {
    Player.status[REGENERATING] -= 1500;
    if (Player.status[REGENERATING] < 1) {
      mprint(LS(IDS_MSG_21447));
      Player.status[REGENERATING] = 0;
    }
  }
}




/* armor functions */


void i_normal_armor(o)
pob o;
{
  if (o->used) mprint(LS(IDS_MSG_21448));
}

void i_perm_energy_resist(o)
pob o;
{
  if (o->used){
    Player.immunity[FLAME]++;
    Player.immunity[COLD]++;
    Player.immunity[ELECTRICITY]++;
  }
  else {
    Player.immunity[FLAME]--;
    Player.immunity[COLD]--;
    Player.immunity[ELECTRICITY]--;
  }
}



void i_perm_fear_resist(o)
pob o;
{
  if (o->used){
    Player.immunity[FEAR]++;
    if (o->blessing < 0) {
      Player.status[BERSERK] += 1500;
      mprint(LS(IDS_MSG_21449));
    }
  }
  else {
    Player.immunity[FEAR]--;
    if (o->blessing < 0) {
      Player.status[BERSERK] -= 1500;
      if (Player.status[BERSERK] < 1) {
	mprint(LS(IDS_MSG_21450));
      Player.status[BERSERK] = 0;
      }
    }
  }
}    



void i_perm_breathing(o)
pob o;
{
  if (o->known < 1)  o->known = 1;
  if (o->blessing > -1)
    Objects[o->id].known = 1;

  if (o->blessing > -1) {
    if (o->used) {
      mprint(LS(IDS_MSG_20784));
      Player.status[BREATHING] += 1500;
    }
    else  {
      Player.status[BREATHING] -= 1500;
      if (Player.status[BREATHING] < 1) {
	mprint(LS(IDS_MSG_21451));
	Player.status[BREATHING] = 0;
      }
    }
  }
  else if (o->used) {
    Player.status[BREATHING] = 0;
    p_drown();
    print1(LS(IDS_MSG_21452));
  }
}



/* weapons functions */

void weapon_acidwhip(dmgmod,o,m)
int dmgmod;
pob o;
struct monster *m;
{
  if ((random_range(2) == 1) && (! m_immunityp(m,NORMAL_DAMAGE))) {
    mprint(LS(IDS_MSG_21453));
    m_status_reset(m,MOBILE);
  }
  p_hit(m,Player.dmg+dmgmod,ACID);

}

void weapon_scythe(dmgmod,o,m)
int dmgmod;
pob o;
struct monster *m;
{
  mprint(LS(IDS_MSG_21454));
  m_death(m);
  if (! Player.rank[ADEPT]) {
    mprint(LS(IDS_MSG_21455));
    mprint(LS(IDS_MSG_21456));
    p_death("the Scythe of Death");
  }
}

void weapon_demonblade(dmgmod,o,m)
int dmgmod;
pob o;
struct monster *m;
{ 
  if (o->blessing > -1) {
    mprint(LS(IDS_MSG_21457));
    mprint(LS(IDS_MSG_20112));
    Player.status[BERSERK] = 0;
    conform_lost_object(o);
  }
  else if (m->specialf == M_SP_DEMON) {
    mprint(LS(IDS_MSG_21458));
    m_vanish(m);
  }
  else if (m->meleef != M_MELEE_SPIRIT) {
    if (m->level > random_range(10)) {
      Player.hp = min(Player.maxhp,Player.hp+m->hp);
      Player.str++;
      Player.pow = min(Player.maxpow,Player.pow+m->level);
      m_death(m);
      mprint(LS(IDS_MSG_21459));
    }
    else p_hit(m,Player.dmg+dmgmod,NORMAL_DAMAGE);

  }
  else {
    mprint(LS(IDS_MSG_21460));
    if (random_range(10) == 1) {
      mprint(LS(IDS_MSG_21461));
      morewait();
      p_damage(50,UNSTOPPABLE,"Demonblade exploding");
      conform_lost_object(o);
    }
    else {
      mprint(LS(IDS_MSG_21462));
      p_damage(25,UNSTOPPABLE,"a backlash of negative energies");
      Player.str -= 3;
      Player.con -= 3;
      if (Player.str < 1 || Player.con < 1)
	p_death("a backlash of negative energies");
    }
  }
}

void weapon_lightsabre(dmgmod,o,m)
int dmgmod;
pob o;
struct monster *m;
{
  if (! o->known) {
    mprint(LS(IDS_MSG_21463));
    p_damage(100,UNSTOPPABLE,"fumbling with a lightsabre");
    o->known = 1;
  }
  else {
    /* test prevents confusing immunity messages.... */
    if (! m_immunityp(m,NORMAL_DAMAGE)) {
      mprint(LS(IDS_MSG_21464));
      m_damage(m,20,NORMAL_DAMAGE);
    }
    if ((m->hp>0) && (! m_immunityp(m,FLAME))) {
      mprint(LS(IDS_MSG_21465));
      m_damage(m,20,FLAME);
    }
  }
}

void weapon_tangle(dmgmod,o,m)
int dmgmod;
pob o;
struct monster *m;
{
  if ((random_range(2) == 1) && (! m_immunityp(m,NORMAL_DAMAGE))) {
    mprint(LS(IDS_MSG_21453));
    m_status_reset(m,MOBILE);
  }
  p_hit(m,Player.dmg+dmgmod,NORMAL_DAMAGE);
}

/* if wielding a bow, add bow damage to arrow damage */
void weapon_arrow(dmgmod,o,m)
int dmgmod;
pob o;
struct monster *m;
{
  if ((Player.possessions[O_WEAPON_HAND] != NULL) &&
      (Player.possessions[O_WEAPON_HAND]->id == WEAPONID+26)) /* ie, using a bow */
    p_hit(m,Player.dmg+o->plus+o->dmg+dmgmod,NORMAL_DAMAGE);
  else p_hit(m,o->plus+o->dmg+dmgmod,NORMAL_DAMAGE);
}

/* if wielding a crossbow, add bow damage to arrow damage */
void weapon_bolt(dmgmod,o,m)
int dmgmod;
pob o;
struct monster *m;
{
  if ((Player.possessions[O_WEAPON_HAND] != NULL) &&
      (Player.possessions[O_WEAPON_HAND]->id == WEAPONID+27) && /*ie using a crossbow */
      (Player.possessions[O_WEAPON_HAND]->aux==LOADED)) {
	p_hit(m,Player.dmg+o->plus+o->dmg+dmgmod,NORMAL_DAMAGE);
	Player.possessions[O_WEAPON_HAND]->aux = UNLOADED;
      }
  else p_hit(m,o->plus+o->dmg,NORMAL_DAMAGE);
}


void weapon_mace_disrupt(dmgmod,o,m)
int dmgmod;
pob o;
struct monster *m;
{
  if (m->meleef == M_MELEE_SPIRIT) {
    mprint(LS(IDS_MSG_21466));
    m_death(m);
  }
  else p_hit(m,Player.dmg+dmgmod,UNSTOPPABLE);
}

void weapon_normal_hit(dmgmod,o,m)
int dmgmod;
pob o;
struct monster *m;
{
  p_hit(m,Player.dmg+dmgmod,NORMAL_DAMAGE);
}


/* will be updated eventually */
void weapon_bare_hands(dmgmod,m)
int dmgmod;
struct monster *m;
{
  p_hit(m,Player.dmg+dmgmod,NORMAL_DAMAGE);
}


void i_demonblade(o)
pob o;
{
  if (o->used) {
    o->known = 2;
    mprint(LS(IDS_MSG_21467));
    mprint(LS(IDS_MSG_21468));
    mprint(LS(IDS_MSG_21469));
    Player.status[BERSERK] = 1500;
  }
  else {
    mprint(LS(IDS_MSG_21470));
    mprint(LS(IDS_MSG_21471));
    Player.status[BERSERK] = 0;
  }
}

void i_normal_weapon(o)
pob o;
{
  if (o->used) mprint(LS(IDS_MSG_21472));
}

void i_lightsabre(o)
pob o;
{
  if (o->used) mprint(LS(IDS_MSG_21473));
  else mprint(LS(IDS_MSG_21474));
}

void i_mace_disrupt(o)
pob o;
{
  mprint(LS(IDS_MSG_21475));
}



void weapon_vorpal(dmgmod,o,m)
int dmgmod;
pob o;
struct monster *m;
{
  if ((random_range(10) < 3) && (! m_immunityp(m,NORMAL_DAMAGE))) {
    o->known = 2;
    if (random_range(2) == 1)
      mprint(LS(IDS_MSG_21476));
    else mprint(LS(IDS_MSG_21477));
    m_death(m);
  }
  else weapon_normal_hit(dmgmod,o,m);
}

void weapon_desecrate(dmgmod,o,m)
int dmgmod;
pob o;
struct monster *m;
{
  o->known = 2;
  if (Player.alignment < 0) {
    mprint(LS(IDS_MSG_21478));
    p_hit(m,Player.dmg+dmgmod,UNSTOPPABLE);
    Player.alignment--;
    if (Player.hp < Player.maxhp) {
      mprint(LS(IDS_MSG_21479));
      Player.hp = min(Player.maxhp,Player.hp+Player.dmg+dmgmod);
    }
  }
  else {
    mprint(LS(IDS_MSG_21480));
    mprint(LS(IDS_MSG_21481));
    level_drain(Player.dmg,"the sword Desecrator");
    Player.alignment-=10;
    mprint(LS(IDS_MSG_21482));
    mprint(LS(IDS_MSG_21483));
  }
}



void weapon_firestar(dmgmod,o,m)
int dmgmod;
pob o;
struct monster *m;
{
  if (random_range(3) == 1) {
    o->known = 2;
    fball(Player.x,Player.y,Player.x,Player.y,max(Player.dmg,25));
  }
  if (m->hp > 0) weapon_normal_hit(dmgmod,o,m);
}

void weapon_defend(dmgmod,o,m)
int dmgmod;
pob o;
struct monster *m;
{
  if ((Player.alignment < 0) && (o->blessing > 0)) {
      mprint(LS(IDS_MSG_21484));
      mprint(LS(IDS_MSG_21485));
      p_damage(50,UNSTOPPABLE,"a pissed-off Holy Defender");
      mprint(LS(IDS_MSG_21486));
      o->truename = o->cursestr;
      Player.status[PROTECTION] -= (o->hit);
      o->plus = 0-abs(o->plus);
      o->blessing = -1;
    }
  if ((o->blessing > 0) &&
      ((m->specialf == M_SP_DEMON) ||
       (m->meleef == M_MELEE_SPIRIT))) {
    mprint(LS(IDS_MSG_21487));
    m->hit = 0;
    m->speed *=2;
  }
  weapon_normal_hit(dmgmod,o,m);
}

void weapon_victrix(dmgmod,o,m)
int dmgmod;
pob o;
struct monster *m;
{
  if (m->meleef == M_MELEE_SPIRIT) {
    mprint(LS(IDS_MSG_21488));
    m_death(m);
  }
  else weapon_normal_hit(dmgmod,o,m);
}

void i_defend(o)
pob o;
{
  o->known = 2;
  if (o->used) {
    mprint(LS(IDS_MSG_21489));
    Player.status[PROTECTION] += o->hit;
  }
  else Player.status[PROTECTION] -= o->hit;
}


void i_victrix(o)
pob o;
{
  o->known = 2;
  o->blessing = abs(o->blessing);
  if (o->used) {
    Player.immunity[POISON]++;
    Player.immunity[FEAR]++;
    Player.immunity[INFECTION]++;
  }
  else {
    Player.immunity[POISON]--;
    Player.immunity[FEAR]--;
    Player.immunity[INFECTION]--;
  }
}    


void i_desecrate(o)
pob o;
{
  if (o->known < 1)  o->known = 2;
  if (o->blessing > 0) {
    mprint(LS(IDS_MSG_21490));
    mprint(LS(IDS_MSG_21491));
    dispose_lost_objects(1,o);
  }
  else if (Level->site[Player.x][Player.y].locchar == ALTAR)
    sanctify(-1);
}


/* shield functions */
void i_normal_shield(o)
pob o;
{
  if (o->used) mprint(LS(IDS_MSG_21492));
}


void i_perm_deflect(o)
pob o;
{
  if (o->known < 1)  o->known = 2;
  if (o->blessing > -1) {
    if (o->used) {
      mprint(LS(IDS_MSG_20925));
      Player.status[DEFLECTION] += 1500;
    }
    else {
      Player.status[DEFLECTION] -= 1500;
      if (Player.status[DEFLECTION] < 1) {
	mprint(LS(IDS_MSG_21493));
	Player.status[DEFLECTION] = 0;
      }
    }
  }
  else {
    if (o->used) {
      mprint(LS(IDS_MSG_21494));
      Player.status[VULNERABLE] += 1500;
      Player.status[DEFLECTION] = 0;
    }
    else {
      Player.status[VULNERABLE] -= 1500;
      if (Player.status[VULNERABLE] < 1) {
	mprint(LS(IDS_MSG_21495));
	Player.status[VULNERABLE] = 0;
      }
    }
  }
}
