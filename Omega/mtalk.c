/* omega copyright (c) 1987,1988,1989 by Laurence Raphael Brothers */
/* mtalk.c */
/* monster talk functions */

#include "glob.h"


/* The druid's altar is in the northern forest */
void m_talk_druid(m)
struct monster *m;
{
  int i;
  pml curr;

  if (! m_statusp(m,HOSTILE)) {
    print1(LS(IDS_MSG_21997));
    if (! gamestatusp(SPOKE_TO_DRUID)) {
      setgamestatus(SPOKE_TO_DRUID);
      morewait();
      print1(LS(IDS_MSG_21998));
      print2(LS(IDS_MSG_21999));
      morewait();
      gain_experience(300);
      if (Player.patron == DRUID) {
	print1(LS(IDS_MSG_22000));
	print2(LS(IDS_MSG_22001));
	morewait();
	for(i=0;i<NUMRANKS;i++) {
	  if (Player.guildxp[i] > 0)
	    Player.guildxp[i] += 300;
	}
      }
    }
    mprint(LS(IDS_MSG_22002));
    if (ynq() == 'y') {
      if (Phase/2 == 6 || Phase/2 == 0) {	/* full or new moon */
	mprint(LS(IDS_MSG_22003));
	if (Phase/2 == 6)
	  mprint(LS(IDS_MSG_22004));
	else
	  mprint(LS(IDS_MSG_22005));
      }
      else if (Phase/2 == 3 || Phase/2 == 9) {	/* half moon */
	mprint(LS(IDS_MSG_22006));
	Player.alignment = 0;
	Player.mana = calcmana();
	if (Player.patron == DRUID)
	  gain_experience(200);	/* if a druid wants to spend 2 days */
	Time+=60;		/* celebrating for 1600 xp, why not? */
	hourly_check();
	Time+=60;
	hourly_check();
	Time+=60;
	hourly_check();
	Time+=60;
	hourly_check();
	Time+=60;
	hourly_check();
	Time+=60;
	hourly_check();
      }
      else {
	mprint(LS(IDS_MSG_22007));
	if (Player.patron == DRUID) {
	  Player.alignment = 0;
	  Player.mana = calcmana();
	}
	else
	  Player.alignment -= Player.alignment*max(0, 10 - Player.level)/10;
	  /* the higher level the character is, the more set in his/her ways */
	Time+=60;
	hourly_check();
      }
      dataprint();
    }
  }
  else {
    mprint(LS(IDS_MSG_22008));
    disrupt(Player.x,Player.y,100);
    mprint(LS(IDS_MSG_22009));
    mprint(LS(IDS_MSG_22010));
    if (ynq()) {
      mprint(LS(IDS_MSG_22011));
      for (curr = Level->mlist; curr; curr = curr->next)
	m_status_reset(curr->m, HOSTILE);
      m_vanish(m);
    }
    else {
      mprint(LS(IDS_MSG_22012));
      p_damage(500,UNSTOPPABLE,"the ArchDruid's Vengeance");
    }
  }
}


void m_talk_silent(m)
struct monster *m;
{

  int reply = random_range(4);
  
  if (m->uniqueness == COMMON) {
    strcpy(Str2,"The ");
    strcat(Str2,m->monstring);
  }
  else strcpy(Str2,m->monstring);
  switch (reply) {
    case 0:strcat(Str2," does not reply. "); break;
    case 1:strcat(Str2," shrugs silently. "); break;
    case 2:strcat(Str2," hold a finger to his mouth. "); break;
    case 3:strcat(Str2," glares at you but says nothing. "); break;
  }      
  mprint(Str2);
}

void m_talk_stupid(m)
struct monster *m;
{

  int reply = random_range(4);
  if (m->uniqueness == COMMON) {
    strcpy(Str2,"The ");
    strcat(Str2,m->monstring);
  }
  else strcpy(Str2,m->monstring);
  switch (reply) {
    case 0:strcat(Str2," looks at you with mute incomprehension."); break;
    case 1:strcat(Str2," growls menacingly and ignores you."); break;
    case 2:strcat(Str2," does not seem to have heard you."); break;
    case 3:strcat(Str2," tries to pretend it didn't hear you."); break;
  }      
  mprint(Str2);
}

void m_talk_greedy(m)
struct monster *m;
{

  int reply = random_range(4);
  if (m->uniqueness == COMMON) {
    strcpy(Str2,"The ");
    strcat(Str2,m->monstring);
  }
  else strcpy(Str2,m->monstring);
  switch (reply) {
    case 0:strcat(Str2," says: Give me a treasure.... ");break;
    case 1:strcat(Str2," says: Stand and deliver, knave! "); break;
    case 2:strcat(Str2," says: Your money or your life! "); break;
    case 3:strcat(Str2," says: Yield or Die! "); break;
  }      
  mprint(Str2);
}

void m_talk_hungry(m)
struct monster *m;
{

  int reply = random_range(4);
  if (m->uniqueness == COMMON) {
    strcpy(Str2,"The ");
    strcat(Str2,m->monstring);
  }
  else strcpy(Str2,m->monstring);
  switch (reply) {
    case 0:strcat(Str2," says: I hunger, foolish adventurer! "); break;
    case 1:strcat(Str2," drools menacingly at you. "); break;
    case 2:strcat(Str2," says: You're invited to be lunch! "); break;
    case 3:strcat(Str2," says: Feeed Meee! "); break;
  }      
  mprint(Str2);
}


void m_talk_guard(m)
struct monster *m;
{
  if (m_statusp(m,HOSTILE)) {
    print1(LS(IDS_MSG_22013));
    print2(LS(IDS_MSG_22014));
    if (ynq2()=='y') {
      Player.alignment++;
      if (Current_Environment == E_CITY) {
	print1(LS(IDS_MSG_22015));
	print2(LS(IDS_MSG_22016));
	morewait();
	send_to_jail();
	drawvision(Player.x,Player.y);
      }
      else {
	clearmsg();
	print1(LS(IDS_MSG_22017));
	dispose_lost_objects(1,Player.possessions[O_WEAPON_HAND]);
	pacify_guards();
      }
    }
    else {
      clearmsg();
      print1(LS(IDS_MSG_22018));
    }
  }
  else if (Player.rank[ORDER]>0) 
    print1(LS(IDS_MSG_22019));
  else print1(LS(IDS_MSG_22020));
}


void m_talk_mp(m)
struct monster *m;
{
  mprint(LS(IDS_MSG_22021));
}


void m_talk_titter(m)
struct monster *m;
{

  if (m->uniqueness == COMMON) {
    strcpy(Str2,"The ");
    strcat(Str2,m->monstring);
  }
  else strcpy(Str2,m->monstring);
  strcat(Str2," titters obscenely at you.");
  mprint(Str2);
}


void m_talk_ninja(m)
struct monster *m;
{
  mprint(LS(IDS_MSG_22022));
  mprint(LS(IDS_MSG_22023));
}




void m_talk_thief(m)
struct monster *m;
{
  if (Player.rank[THIEVES]) {
    if (m->level == 2) 
      m->monstring = "sneak thief";
    else m->monstring = "master thief";
    print1(LS(IDS_MSG_22024));
    print2(LS(IDS_MSG_22025));
    print3(LS(IDS_MSG_22026));
    morewait();
    m_vanish(m);
  }
  else m_talk_man(m);

}



void m_talk_assassin(m)
struct monster *m;
{
  m->monstring = "master assassin";
  print1(LS(IDS_MSG_22027));
  print2(LS(IDS_MSG_22028));
}
    

void m_talk_im(m)
struct monster *m;
{
  if (strcmp(m->monstring,"itinerant merchant") != 0) {
    m->monstring = "itinerant merchant";
  }
  if (m->possessions == NULL)
    mprint(LS(IDS_MSG_22029));
  else {
    m->possessions->thing->known = 2;
    clearmsg();
    mprint(LS(IDS_MSG_22030));
    mprint(itemid(m->possessions->thing));
    mprint(LS(IDS_MSG_22031));
    mlongprint(max(10,4*true_item_value(m->possessions->thing)));
    mprint(LS(IDS_MSG_21143));
    mprint(LS(IDS_MSG_22032));
    if (ynq()=='y') {
      if (Player.cash < (max(10,4*true_item_value(m->possessions->thing)))) {
	if (Player.alignment > 10) {
	  mprint(LS(IDS_MSG_22033));
	  Player.cash = 0;
	  gain_item(m->possessions->thing);
	  m->possessions = NULL;
	}
	else mprint(LS(IDS_MSG_22034));
      }
      else {
	mprint(LS(IDS_MSG_22035));
	Player.cash -= max(10,(4*item_value(m->possessions->thing)));
	gain_item(m->possessions->thing);
	m->possessions = NULL;
      }
    }
    else mprint(LS(IDS_MSG_22036));
    m_vanish(m);
  }
}


void m_talk_man(m)
struct monster *m;
{

  if (m->uniqueness == COMMON) {
    strcpy(Str2,"The ");
    strcat(Str2,m->monstring);
  }
  else strcpy(Str2,m->monstring);
  switch (random_range(5)) {
    case 0:strcat(Str2," asks you for the way home."); break;
    case 1:strcat(Str2," wishes you a pleasant day."); break;
    case 2:strcat(Str2," sneers at you contemptuously."); break;
    case 3:strcat(Str2," smiles and nods."); break;
    case 4:strcat(Str2," tells you a joke."); break;
  }
  mprint(Str2);
}


void m_talk_evil(m)
struct monster *m;
{

  if (m->uniqueness == COMMON) {
    strcpy(Str2,"The ");
    strcat(Str2,m->monstring);
  }
  else strcpy(Str2,m->monstring);
  switch (random_range(14)) {
    case 0:strcat(Str2," says: 'THERE CAN BE ONLY ONE!'"); break;
    case 1:strcat(Str2," says: 'Prepare to die, Buckwheat!'"); break;
    case 2:strcat(Str2," says: 'Time to die!'"); break;
    case 3:strcat(Str2," says: 'There will be no mercy.'"); break;
    case 4:strcat(Str2," insults your mother-in-law."); break;
    case 5:strcat(Str2," says: 'Kurav tu ando mul!'");
    case 6:strcat(Str2," says: '!va al infierno!'"); break;
    case 7:strcat(Str2," says: 'dame desu, nee.'"); break;
    case 8:strcat(Str2," spits on your rug and calls your cat a bastard."); 
      break;
    case 9:strcat(Str2," snickers malevolently and draws a weapon."); break;
    case 10:strcat(Str2," sends 'rm -r *' to your shell!"); break;
    case 11:strcat(Str2," tweaks your nose and cackles evilly."); break;
    case 12:strcat(Str2," thumbs you in the eyes."); break;
    case 13:strcat(Str2," kicks you in the groin."); break;
  }
  mprint(Str2);
}


void m_talk_robot(m)
struct monster *m;
{
  if (m->uniqueness == COMMON) {
    strcpy(Str2,"The ");
    strcat(Str2,m->monstring);
  }
  else strcpy(Str2,m->monstring);
  switch (random_range(4)) {
    case 0:strcat(Str2," says: 'exterminate...Exterminate...EXTERMINATE!!!'");
      break;
    case 1:strcat(Str2," says: 'Kill ... Crush ... Destroy'");
      break;
    case 2:strcat(Str2," says: 'Danger -- Danger'");
      break;
    case 3:strcat(Str2," says: 'Yo Mama -- core dumped.'");
      break;
  }
  mprint(Str2);
}

void m_talk_slithy(m)
struct monster *m;
{
  mprint(LS(IDS_MSG_22037));
}


void m_talk_mimsy(m)
struct monster *m;
{
  mprint(LS(IDS_MSG_22038));
}



void m_talk_burble(m)
struct monster *m;
{

  if (m->uniqueness == COMMON) {
    strcpy(Str2,"The ");
    strcat(Str2,m->monstring);
  }
  else strcpy(Str2,m->monstring);
  strcat(Str2," burbles hatefully at you.");
  mprint(Str2);
}




void m_talk_beg(m)
struct monster *m;
{
  if (m->uniqueness == COMMON) {
    strcpy(Str2,"The ");
    strcat(Str2,m->monstring);
  }
  else strcpy(Str2,m->monstring);
  strcat(Str2," asks you for alms.");
  mprint(Str2);
}


void m_talk_hint(m)
struct monster *m;
{
  if (m->uniqueness == COMMON) {
    strcpy(Str2,"The ");
    strcat(Str2,m->monstring);
  }
  else strcpy(Str2,m->monstring);
  if (m_statusp(m,HOSTILE)) {
    strcat(Str2," only sneers at you. ");
    mprint(Str2);
  }
  else {
    strcat(Str2," whispers in your ear: ");
    mprint(Str2);
    hint();
    m->talkf = M_TALK_SILENT;
  }
}

void m_talk_gf(m)
struct monster *m;
{
  mprint(LS(IDS_MSG_22039));
  if (ynq()=='y') {
    mprint(LS(IDS_MSG_22040));
    if (ynq()=='y') {
      mprint(LS(IDS_MSG_22041));
      if (ynq()=='y') {
	mprint(LS(IDS_MSG_22042));
	if (ynq()=='y') {
	  mprint(LS(IDS_MSG_22043));
	  if (ynq()=='y') mprint(LS(IDS_MSG_22044));
	  else wish(0);
	}
      }
    }
  }
  mprint(LS(IDS_MSG_22045));
  Player.hp = max(Player.hp,Player.maxhp);
  Player.mana = max(Player.mana,calcmana());
  mprint(LS(IDS_MSG_22046));
  m_vanish(m);
}

void m_talk_ef(m)
struct monster *m;
{
  mprint(LS(IDS_MSG_22047));
  mprint(LS(IDS_MSG_22048));
  m->movef=M_MOVE_SMART;
  m->meleef=M_MELEE_POISON;
  m->specialf=M_SP_THIEF;
  acquire(-1);
  bless(-1);
  sleep_player(m->level/2);
  summon(-1,-1);
  summon(-1,-1);
  summon(-1,-1);
  summon(-1,-1);
}


void m_talk_seductor(m)
struct monster *m;
{
  if (m->uniqueness == COMMON) {
    strcpy(Str2,"The ");
    strcat(Str2,m->monstring);
  }
  else strcpy(Str2,m->monstring);
  if (Player.preference == 'n')
  {
    strcat(Str2, " notices your disinterest and leaves with a pout.");
    mprint(Str2);
  }
  else
  {
    strcat(Str2," beckons seductively...");
    mprint(Str2);
    mprint(LS(IDS_MSG_22049));
    if (ynq()=='y') {
      mprint(LS(IDS_MSG_20752));
    }
    else {
      strcpy(Str2,"The ");
      strcat(Str2,m->monstring);
      strcat(Str2," shows you a good time....");
      mprint(Str2);
      gain_experience(500);
      Player.con++;
    }
  }
  m_vanish(m);
}


void m_talk_demonlover(m)
struct monster *m;
{
  if (m->uniqueness == COMMON) {
    strcpy(Str2,"The ");
    strcat(Str2,m->monstring);
  }
  else strcpy(Str2,m->monstring);
  if (Player.preference == 'n')
  {
    strcat(Str2, " notices your disinterest and changes with a snarl...");
    mprint(Str2);
    morewait();
  }
  else
  {
    strcat(Str2," beckons seductively...");
    mprint(Str2);
    mprint(LS(IDS_MSG_22049));
    if (ynq()=='y') 
      mprint(LS(IDS_MSG_22050));
    else {
      if (m->uniqueness == COMMON) {
	strcpy(Str2,"The ");
	strcat(Str2,m->monstring);
      }
      else strcpy(Str2,m->monstring);
      strcat(Str2," shows you a good time....");
      mprint(Str2);
      morewait();
      mprint(LS(IDS_MSG_22051));
      level_drain(random_range(3)+1,"a demon's kiss");
      morewait();
    }
  }
  m->talkf = M_TALK_EVIL;
  m->meleef = M_MELEE_SPIRIT;
  m->specialf = M_SP_DEMON;

  if ((m->monchar&0xff) == 's') {
    m->monchar = 'I'|COL_RED;
    m->monstring = "incubus";
  }
  else {
    m->monchar = 'S'|COL_RED;
    m->monstring = "succubus";
  }
  if (m->uniqueness == COMMON) {
    strcpy(Str2,"The ");
    strcat(Str2,m->monstring);
  }
  else strcpy(Str2,m->monstring);
  strcat(Str2," laughs insanely.");
  mprint(Str2);
  mprint(LS(IDS_MSG_22052));
}


void m_talk_horse(m)
struct monster *m;
{
  if (m_statusp(m,HOSTILE)) 
    mprint(LS(IDS_MSG_22053));
  else if (m_statusp(m,HUNGRY))
    mprint(LS(IDS_MSG_22054));
  else if (gamestatusp(MOUNTED))
    mprint(LS(IDS_MSG_22055));
  else if (Current_Environment == Current_Dungeon)
    mprint(LS(IDS_MSG_22056));
  else {
    mprint(LS(IDS_MSG_22057));
    if (ynq()=='y') {
      m->hp = -1;
      Level->site[m->x][m->y].creature = NULL;
      putspot(m->x,m->y,getspot(m->x,m->y,FALSE));
      setgamestatus(MOUNTED);
      calc_melee();
      mprint(LS(IDS_MSG_22058));
    }
  }
}

void m_talk_hyena(m)
struct monster *m;
{
  mprint(LS(IDS_MSG_22059));
}

void m_talk_parrot(m)
struct monster *m;
{
  mprint(LS(IDS_MSG_22060));
}


void m_talk_servant(m)
struct monster *m;
{
  int target,x=Player.x,y=Player.y;
  if (m->id == ML4+12) {
    target = ML4+13;
    mprint(LS(IDS_MSG_22061));
    mprint(LS(IDS_MSG_22062));
  }
  else {
    target = ML4+12;
    mprint(LS(IDS_MSG_22063));
    mprint(LS(IDS_MSG_22064));
  }
  if (ynq()=='y') {
    print1(LS(IDS_MSG_22065));
    show_screen();
    drawmonsters(TRUE);
    setspot(&x,&y);
    if (Level->site[x][y].creature != NULL) {
      if (Level->site[x][y].creature->id == target) {
	mprint(LS(IDS_MSG_22066));
	mprint(LS(IDS_MSG_22067));
	gain_experience(m->xpv);
	m_death(Level->site[x][y].creature);
	Level->site[m->x][m->y].creature = NULL;
	m->x = x;
	m->y = y;
	Level->site[x][y].creature = m;
	m_death(Level->site[x][y].creature);
      }
      else mprint(LS(IDS_MSG_22068));
    }
    else mprint(LS(IDS_MSG_22068));
  }
  else mprint(LS(IDS_MSG_22069));
}


void m_talk_animal(m)
struct monster *m;
{
  if (m->uniqueness == COMMON) {
    strcpy(Str2,"The ");
    strcat(Str2,m->monstring);
  }
  else strcpy(Str2,m->monstring);
  mprint(Str2);
  mprint(LS(IDS_MSG_22070));
  mprint(LS(IDS_MSG_22071));
  mprint(LS(IDS_MSG_22072));
  mprint(LS(IDS_MSG_22073));
}


void m_talk_scream(m)
struct monster *m;
{
  mprint(LS(IDS_MSG_22074));
  morewait();
  mprint(LS(IDS_MSG_22075));
  morewait();
  mprint(LS(IDS_MSG_22076));
  summon(-1,QUAIL);
  m->talkf = M_TALK_EVIL;
}


void m_talk_archmage(m)
struct monster *m;
{
  if (m_statusp(m,HOSTILE)) {
    mprint(LS(IDS_MSG_22077));
    mprint(LS(IDS_MSG_22078));
  }
  else if (Current_Environment == E_COURT) {
    mprint(LS(IDS_MSG_22079));
    mprint(LS(IDS_MSG_22080));
    mprint(LS(IDS_MSG_22081));
    mprint(LS(IDS_MSG_22082));
    if (Level->site[m->x][m->y].p_locf == L_THRONE) {
      mprint(LS(IDS_MSG_22083));
      m_vanish(m);
    }
  }
  else {
    mprint(LS(IDS_MSG_22084));
    mprint(LS(IDS_MSG_22085));
    mprint(LS(IDS_MSG_22086));
    mprint(LS(IDS_MSG_22087));
  }
}


void m_talk_merchant(m)
struct monster *m;
{
  if (! m_statusp(m,HOSTILE)) {
    if (Current_Environment == E_VILLAGE) {
      mprint(LS(IDS_MSG_22088));
      mprint(LS(IDS_MSG_22089));
      if (ynq()=='y') {
	if (Player.cash < 250) 
	  mprint(LS(IDS_MSG_22090));
	else {
	  Player.cash -= 250;
	  mprint(LS(IDS_MSG_22091));
	  mprint(LS(IDS_MSG_22092));
	  mprint(LS(IDS_MSG_22093));
	  mprint(LS(IDS_MSG_22094));
	  mprint(LS(IDS_MSG_22095));
	  m_vanish(m);
	}
      }
      else mprint(LS(IDS_MSG_22096));
    }
    else {
      mprint(LS(IDS_MSG_22097));
      mprint(LS(IDS_MSG_22098));
    }
  }
  else {
    mprint(LS(IDS_MSG_22099));
    mprint(LS(IDS_MSG_22100));
  }
}
  

void m_talk_prime(m)
struct monster *m;
{
  if (!m_statusp(m,HOSTILE)) {
    if (Current_Environment == E_CIRCLE) {
      print1(LS(IDS_MSG_22101));
      print2(LS(IDS_MSG_22102));
      morewait();
      m_dropstuff(m);
      m_vanish(m);
    }
    else {
      print1(LS(IDS_MSG_22103));
      print2(LS(IDS_MSG_22104));
      if (Player.rank[CIRCLE] > 0) {
	morewait();
	print1(LS(IDS_MSG_22105));
	print2(LS(IDS_MSG_22106));
	Player.pow+=Player.rank[CIRCLE];
	Player.mana += calcmana();
	gain_experience(1000);
	m_vanish(m);
      }
    }
  }
  else m_talk_evil(m);
}
