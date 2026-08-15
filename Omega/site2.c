/* omega copyright (C) by Laurence Raphael Brothers, 1987,1988,1989 */
/* site2.c */
/* 2nd half of site functions and aux functions to them */

#include "glob.h"



void l_condo()
{
  pol ol,prev=NULL;
  int i,done=FALSE,over=FALSE,weeksleep=FALSE;
  char response;

  if (! gamestatusp(SOLD_CONDO)) {
    print1(LS(IDS_MSG_22385));
    print2(LS(IDS_MSG_22386));
    response = mgetc();
    if (response == 'p') {
      print2(LS(IDS_MSG_22387));
      if (ynq2()=='y') {
	if (Player.cash < 50000) 
	  print3(LS(IDS_MSG_22388));
	else {
	  setgamestatus(SOLD_CONDO);
	  Player.cash-=50000;
	  dataprint();
	  print2(LS(IDS_MSG_22389));
	  Condoitems = NULL;
	}
      }
    }
    else if (response == 'r') {
      print2(LS(IDS_MSG_22390));
      if (ynq2()=='y') {
	if (Player.cash < 1000)
	  print2(LS(IDS_MSG_22391));
	else {
	  weeksleep = TRUE;
	  Player.cash -=1000;
	  dataprint();
	}
      }
    }
    else print2(LS(IDS_MSG_22392));
  }
  else {
    while (! done) {
      menuclear();
      menuprint("Home Sweet Home\n");
      menuprint("a: Leave items in your safe.\n");
      menuprint("b: Retrieve items.\n");
      menuprint("c: Take a week off to rest.\n");
      menuprint("d: Retire permanently.\n");
      menuprint("ESCAPE: Leave this place.\n");
      showmenu();
      response = (char) mcigetc();
      if (response == 'a') {
	i = getitem(NULL_ITEM);
	if (i != ABORT) {
	  if (Player.possessions[i]->blessing < 0) 
	    print2(LS(IDS_MSG_22393));
	  else {
	    ol = ((pol) checkmalloc(sizeof(oltype)));
	    ol->thing = Player.possessions[i];
	    ol->next = Condoitems;
	    Condoitems = ol;
	    conform_unused_object(Player.possessions[i]);
	    Player.possessions[i] = NULL;
	  }
	}
      }
      else if (response == 'b') {
	ol = Condoitems;
	while ((ol != NULL) && (! over)) {
	  print1(LS(IDS_MSG_22394));
	  nprint1(itemid(ol->thing));
	  nprint1(LS(IDS_MSG_22395));
	  response = (char) mcigetc();
	  if (response == 'y') {
	    gain_item(ol->thing);
	    if (ol == Condoitems)
	      Condoitems = Condoitems->next;
	    else if (prev != NULL) prev->next = ol->next;
	  }
	  else if (response == 'q')
	    over = TRUE;
	  prev = ol;
	  ol = ol->next;
	}
      }
      else if (response == 'c') weeksleep = TRUE;
      else if (response == 'd') {
	clearmsg();
	print1(LS(IDS_MSG_22396));
	if (ynq1() == 'y') {
	  p_win();
	}
      }
      else if (response == ESCAPE) done = TRUE;
    }
    xredraw();
  }
  if (weeksleep) {
    clearmsg();
    print1(LS(IDS_MSG_22397));
    morewait();
    toggle_item_use(TRUE);
    Player.hp = Player.maxhp;
    Player.str = Player.maxstr;
    Player.agi = Player.maxagi;
    Player.con = Player.maxcon;
    Player.dex = Player.maxdex;
    Player.iq = Player.maxiq;
    Player.pow = Player.maxpow;
    for (i=0;i<NUMSTATI;i++)
      if (Player.status[i]<1000) Player.status[i]=0;
    toggle_item_use(FALSE);
    Player.food = 36;
    print2(LS(IDS_MSG_22398));
    Time += 60*24*7;
    Date += 7;
    moon_check();
    timeprint();
  }
}




void gymtrain(maxstat,stat)
int *maxstat,*stat;
{
  if (Gymcredit + Player.cash < 2000)
    print2(LS(IDS_MSG_22399));
  else {
    if (Gymcredit > 2000) Gymcredit -= 2000;
    else {
      Player.cash -= (2000-Gymcredit);
      Gymcredit = 0;
    }
    print2(LS(IDS_MSG_22400));
    if ((*maxstat < 30) && 
	((*maxstat < random_range(30)) || (random_range(3)==1))){
      nprint2(LS(IDS_MSG_22401));
      (*maxstat)++;
      (*stat)++;
    }
    else {
      nprint2(LS(IDS_MSG_22402));
      if (*stat < *maxstat) {
	(*stat)++;
	print3(LS(IDS_MSG_22403));
      }
    }
  }
  dataprint();
}

void healforpay()
{
  if (Player.cash < 50)
    print2(LS(IDS_MSG_22404));
  else {
    Player.cash -= 50;
    Player.hp += 20+random_range(20);
    if (Player.hp > Player.maxhp) 
      Player.hp = Player.maxhp;
    print2(LS(IDS_MSG_22405));
  }
  calc_melee();
}

void cureforpay()
{
  if (Player.cash < 250)
    print2(LS(IDS_MSG_22406));
  else {
    Player.cash -= 250;
    Player.status[DISEASED] = 0;
    print2(LS(IDS_MSG_22407));
  }
}  



void pacify_guards()
{
  pml ml;

  for(ml=Level->mlist;ml!=NULL;ml=ml->next)
    if ((ml->m->id == ML0+3) || /*guard*/
	((ml->m->id == ML0+8) && (ml->m->aux2 == 15))) {/* justiciar */
      m_status_reset(ml->m,HOSTILE);
      ml->m->specialf = M_NO_OP;
      if (ml->m->id == ML0+3 && ml->m->hp > 0 && ml->m->aux1 > 0) {
	if (Level->site[ml->m->x][ml->m->y].creature == ml->m)
	  Level->site[ml->m->x][ml->m->y].creature = NULL;
	ml->m->x = ml->m->aux1;
	ml->m->y = ml->m->aux2;
	Level->site[ml->m->x][ml->m->y].creature = ml->m;
      }
      else if (ml->m->id == ML0+8 && ml->m->hp > 0 &&
	  Current_Environment == E_CITY) {
	if (Level->site[ml->m->x][ml->m->y].creature == ml->m)
	  Level->site[ml->m->x][ml->m->y].creature = NULL;
	ml->m->x = 40;
	ml->m->y = 62;
	Level->site[ml->m->x][ml->m->y].creature = ml->m;
      }
    }
    if (Current_Environment == E_CITY)
      Level->site[40][60].p_locf = L_ORDER; /* undoes action in alert_guards */
}

void send_to_jail()
{
  if (Player.rank[ORDER] > 0) {
    print1(LS(IDS_MSG_22408));
    print2(LS(IDS_MSG_22409));
    morewait();
    print1(LS(IDS_MSG_22410));
    print2(LS(IDS_MSG_22411));
    Player.rank[ORDER] = -1;
  }
  else if (gamestatusp(DESTROYED_ORDER)) 
    print1(LS(IDS_MSG_22412));
  else if ((Current_Environment != E_CITY) &&
	   (Last_Environment != E_CITY)) 
    print1(LS(IDS_MSG_22413));
  else {
    pacify_guards();
    if (((Current_Environment == E_HOUSE) || 
	 (Current_Environment == E_MANSION) ||
	 (Current_Environment == E_HOVEL)) &&
	(Last_Environment == E_CITY))
    {
      setgamestatus(SUPPRESS_PRINTING);
      change_environment(E_CITY);
      resetgamestatus(SUPPRESS_PRINTING);
    }
    if (Current_Environment == E_CITY) {
      if (gamestatusp(UNDEAD_GUARDS)) {
	print1(LS(IDS_MSG_22414));
	print2(LS(IDS_MSG_22415));
	morewait();
	print1(LS(IDS_MSG_22416));
	print2(LS(IDS_MSG_22417));
	morewait();
	clearmsg();
	print1(LS(IDS_MSG_22418));
	morewait();
	nprint1(LS(IDS_MSG_22419));
	morewait();
	nprint1(LS(IDS_MSG_22419));
	morewait();
	nprint1(LS(IDS_MSG_22420));
	clearmsg();
	print1(LS(IDS_MSG_22421));
	print2(LS(IDS_MSG_22422));
	print3(LS(IDS_MSG_22423));
	while(Player.level > 0) {
	  Player.level--;
	  Player.xp /= 2;
	  Player.hp /= 2;
	  dataprint();
	}
	Player.maxhp = Player.maxcon;
	morewait();
	print1(LS(IDS_MSG_22424));
	Player.y = 58; // _WIN32 fix
	Player.x = 40; // _WIN32 fix
	screencheck(58);
      }
      else if (Player.alignment + random_range(200) < 0) {
	print1(LS(IDS_MSG_22425));
	print2(LS(IDS_MSG_22426));
	Player.y = 58; // _WIN32 fix
	Player.x = 40; // _WIN32 fix
	screencheck(58);
      }
      else switch(Imprisonment++) {
      case 0:
	print1(LS(IDS_MSG_22427));
	print2(LS(IDS_MSG_22428));
	Player.y = 58;
	Player.x = 40;
	screencheck(58);
	break;
      case 1:
	print1(LS(IDS_MSG_22429));
	print2(LS(IDS_MSG_22430));
	morewait();	
	repair_jail();
	Player.y = 54;
	Player.x = 37 + (2*random_range(4));
	screencheck(54);
	l_portcullis_trap();
	break;
      default:
	print1(LS(IDS_MSG_22431));
	print2(LS(IDS_MSG_22432));
	morewait();
	print1(LS(IDS_MSG_22433)); 
	print2(LS(IDS_MSG_22434));
	p_damage(random_range(Imprisonment * 10),
		 UNSTOPPABLE,
		 "police brutality");
	morewait();
	repair_jail();
	Player.y = 54;
	Player.x = 37 + (2*random_range(4));
	screencheck(54);
	l_portcullis_trap();
      }
    }
  }
}






void l_adept()
{
  print1(LS(IDS_MSG_22435));
  if (! gamestatusp(ATTACKED_ORACLE)) {
    if (Player.str+Player.con+Player.iq+Player.pow < 100)
      print2(LS(IDS_MSG_22436));
    else print2(LS(IDS_MSG_22437));
    morewait();
    clearmsg();
  }
  print2(LS(IDS_MSG_22438));
  if (ynq2()!='y') {
    if (Player.level > 100) {
      print1(LS(IDS_MSG_22439));
      Player.xp = 0;
      Player.level = 0;
      Player.hp = Player.maxhp = Player.con;
      Player.mana = calcmana();
      print2(LS(IDS_MSG_22440));
      dataprint();
    }	
  }
  else {
    clearmsg();
    print1(LS(IDS_MSG_22441));
    morewait();
    drawomega();
    print1(LS(IDS_MSG_22442));
    if (Player.patron != DESTINY) {
      print2(LS(IDS_MSG_22443));
      p_damage(random_range(200),UNSTOPPABLE,"a vortex of chaos");
    }
    else print2(LS(IDS_MSG_22444));
    morewait();
    print1(LS(IDS_MSG_22445));
    print2(LS(IDS_MSG_22446));
    morewait();
    Player.hp = Player.maxhp;
    Player.mana = calcmana();
    change_environment(E_ABYSS);
  }
}




void l_trifid()
{
  int damage=0,stuck=TRUE;
  print1(LS(IDS_MSG_22447));
  while (stuck) {
    dataprint();
    damage += Level->depth/2+1;
    print2(LS(IDS_MSG_22448));
    morewait();
    if (find_and_remove_item(THINGID+6,-1)) {
      print1(LS(IDS_MSG_22449));
      print2(LS(IDS_MSG_22450));
      Level->site[Player.x][Player.y].locchar = FLOOR;
      Level->site[Player.x][Player.y].p_locf = L_NO_OP;
      lset(Player.x, Player.y, CHANGED);
      gain_experience(1000);
      stuck = FALSE;
    }
    else {
      p_damage(damage,UNSTOPPABLE,"a trifid");
      morewait();
      print1(LS(IDS_MSG_22451));
      menuclear();
      menuprint("a: Try to break free.\n");
      menuprint("b: Hang limp and hope the tendrils uncoil.\n");
      menuprint("c: Pray for assistance.\n");
      menuprint("d: Attempt to bargain with the hedge.\n");
      menuprint("e: Click your heels together and wish for escape.\n");
      menuprint("ANYTHING ELSE: writhe and scream hopelessly.\n");
      showmenu();
      switch(menugetc()) {
      case 'a': 
	if (Player.str > random_range(200)) {
	  print1(LS(IDS_MSG_22452));
	  print2(LS(IDS_MSG_22453));
	  stuck = FALSE;
	}
	else print1(LS(IDS_MSG_22454));
	break;
      case 'b': 
	print1(LS(IDS_MSG_22455));
	break;
      case 'c': 
	if ((Player.patron == DRUID) && 
	    (Player.rank[PRIESTHOOD] > random_range(5))) {
	    print1(LS(IDS_MSG_22456));
	    print2(LS(IDS_MSG_22457));
	    stuck = FALSE;
	  }
	else print1(LS(IDS_MSG_22458));
	break;
      case 'd':
	print1(LS(IDS_MSG_22459));
	break;
      case 'e':
	print1(LS(IDS_MSG_22460));
	break;
      default:
	print1(LS(IDS_MSG_22461));
	break;
      }
    }
  }
  xredraw();
}




void l_vault()
{
  print1(LS(IDS_MSG_22462));
  if ((hour()==23)) {
    print2(LS(IDS_MSG_22463));
    Level->site[12][56].locchar = FLOOR;
  }
  else {
    print2(LS(IDS_MSG_22464));
    Level->site[12][56].locchar = WALL;
    morewait();
    clearmsg();
    print1(LS(IDS_MSG_22465));
    if (ynq1()=='y') {
      if (random_range(100) < Player.rank[THIEVES]*Player.rank[THIEVES]) {
	print2(LS(IDS_MSG_22466));
	gain_experience(5000);
	Level->site[12][56].locchar = FLOOR;
      }	
      else {
	print2(LS(IDS_MSG_22467));
	morewait();
	if (Player.rank[NOBILITY] == DUKE) {
	  clearmsg();
	  print1(LS(IDS_MSG_22468));
	  morewait();
	}
	else
	  send_to_jail();
      }
    }
    else print2(LS(IDS_MSG_21556));
  }
}


void l_brothel()
{
  char response;
  print1(LS(IDS_MSG_22469));
  print2(LS(IDS_MSG_22470));
  morewait();
  clearmsg();
  print1(LS(IDS_MSG_22471));
  if (ynq1()=='y') {
    menuclear();
    menuprint("a:knock on the door.\n");
    menuprint("b:try to pick the lock.\n");
    menuprint("c:bash down the door.\n");
    menuprint("ESCAPE: Leave this house of ill repute.\n");
    showmenu();
    do 
      response = menugetc();
    while ((response != 'a') &&
	   (response != 'b') &&
	   (response != 'c') &&
	   (response != ESCAPE));
    xredraw();
    if (response == 'a') {
      if (!nighttime())
	print2(LS(IDS_MSG_22472));
      else {
	print1(LS(IDS_MSG_22473));
	print2(LS(IDS_MSG_22474));
	if (ynq2()=='y') {
	  if (Player.cash < 500) {
	    print1(LS(IDS_MSG_22475));
	    print2(LS(IDS_MSG_22476));
	    p_damage(25,UNSTOPPABLE,"da bouncer");
	  }
	  else {
	    Player.cash -= 500;
	    print1(LS(IDS_MSG_22477));
	    print2(LS(IDS_MSG_22478));
	    morewait();
	    if (Player.preference == 'n') {
	      switch(random_range(4)) {
	      case 0:
		print1(LS(IDS_MSG_22479));
		break;
	      case 1:
		print1(LS(IDS_MSG_22480));
		break;
	      case 2:
		print1(LS(IDS_MSG_22481));
		break;
	      case 3:
		print1(LS(IDS_MSG_22482));
	      }
	      print2(LS(IDS_MSG_22483));
	    }
	    else {
	      print1(LS(IDS_MSG_22484));
	      if (Player.preference == 'm' ||
		  (Player.preference == 'y' && random_range(2)))
		switch(random_range(4)) {
		case 0: 
		  print2(LS(IDS_MSG_22485));
		  break;
		case 1: 
		  print2(LS(IDS_MSG_22486));
		  break;
		case 2: 
		  print2(LS(IDS_MSG_22487));
		  break;
		case 3: 
		  print2(LS(IDS_MSG_22488));
		  break;
		}	
	      else switch(random_range(4)) {
	      case 0: 
		print2(LS(IDS_MSG_22489));
		break;
	      case 1: 
		print2(LS(IDS_MSG_22490));
		break;
	      case 2: 
		print2(LS(IDS_MSG_22491));
		break;
	      case 3: 
		print2(LS(IDS_MSG_22492));
		break;
	      }	
	    }
	    morewait();
	    if (hour() > 12) Time += ((24-hour())+8) * 60;
	    else {
	      Time += ((9-hour())*60);
	      Date++;
	    }
	    Player.food = 40;
	    Player.status[DISEASED] = 0;
	    Player.status[POISONED] = 0;
	    Player.hp = Player.maxhp;
	    /* reduce temporary stat gains to max stat levels */
	    toggle_item_use(TRUE);
	    Player.str = min(Player.str,Player.maxstr);
	    Player.con = min(Player.con,Player.maxcon);
	    Player.agi = min(Player.agi,Player.maxagi);
	    Player.dex = min(Player.dex,Player.maxdex);
	    Player.iq = min(Player.iq,Player.maxiq);
	    Player.pow = min(Player.pow,Player.maxpow);
	    toggle_item_use(FALSE);
	    if (Player.preference == 'n')
	      Player.iq++; /* whatever :-) */
	    else
	      Player.con++;
	    gain_experience(100);
	    timeprint();
	    dataprint();
	    showflags();
	    morewait();
	    clearmsg();
	    if (Player.preference == 'n')
	      print1(LS(IDS_MSG_22493));
	    else
	      print1(LS(IDS_MSG_22494));
	  }
	}
	else print2(LS(IDS_MSG_22495));
      }
    }
    else if (response == 'b') {
      if (nighttime()) {
	print1(LS(IDS_MSG_22496));
	print2(LS(IDS_MSG_22497));
      }
      else print1(LS(IDS_MSG_22498));
    }
    else if (response == 'c') {
      if (nighttime()) {
	print1(LS(IDS_MSG_22499));
	print2(LS(IDS_MSG_22500));
	morewait();
	print1(LS(IDS_MSG_22501));
	p_damage(20,UNSTOPPABLE,"a move worthy of Clouseau");
	print2(LS(IDS_MSG_22497));
      }
      else {
	print1(LS(IDS_MSG_22502));
	p_damage(1,UNSTOPPABLE,"a sturdy door");
	morewait();
	print1(LS(IDS_MSG_22503));
	print2(LS(IDS_MSG_22504));
      }
    }
  }
}



#ifndef MSDOS
/* if signp is true, always print message, otherwise do so only sometimes */
void sign_print(x,y,signp)
int x,y,signp;
{
  if ((Level->site[x][y].p_locf >= CITYSITEBASE) &&
      (Level->site[x][y].p_locf < CITYSITEBASE+NUMCITYSITES))
    CitySiteList[Level->site[x][y].p_locf - CITYSITEBASE][0] = TRUE;
  switch(Level->site[x][y].p_locf) {
  case L_CHARITY:
    print1(LS(IDS_MSG_21907));
    break;
  case L_MANSION:
    print1(LS(IDS_MSG_21908));
    print2(LS(IDS_MSG_21909));
    morewait();
    break;
  case L_GRANARY:  
    print1(LS(IDS_MSG_21908));
    print2(LS(IDS_MSG_21910));
    break;
  case L_PORTCULLIS:
    if (Level->site[x][y].locchar == FLOOR)
      print1(LS(IDS_MSG_21911));
    break;
  case L_STABLES:
    print1(LS(IDS_MSG_21908));
    print2(LS(IDS_MSG_21912));
    break;
  case L_COMMONS:
    print1(LS(IDS_MSG_21908));
    print2(LS(IDS_MSG_21913));
    break;
  case L_MAZE:
    print1(LS(IDS_MSG_21908));
    print2(LS(IDS_MSG_21914));
    break;
  case L_BANK:
    if (signp) {
      print1(LS(IDS_MSG_21908));
      print2(LS(IDS_MSG_21915));
    }
    break;
  case L_TEMPLE:
    print1(LS(IDS_MSG_21916));
    break;
  case L_ARMORER:
    if (signp) {
      print1(LS(IDS_MSG_21908));
      print2(LS(IDS_MSG_21917));
    }
    break;
  case L_CLUB:
    if (signp) {
      print1(LS(IDS_MSG_21908));
      print2(LS(IDS_MSG_21918));
    }
    break;
  case L_GYM:
    if (signp) {
      print1(LS(IDS_MSG_21908));
      print2(LS(IDS_MSG_21919));
    }
    break;
  case L_HEALER:
    if (signp) {
      print1(LS(IDS_MSG_21908));
      print2(LS(IDS_MSG_21920));
    }
    break;
  case L_CASINO:
    if (signp) {
      print1(LS(IDS_MSG_21908));
      print2(LS(IDS_MSG_21921));
    }
    break;
  case L_SEWER:
    print1(LS(IDS_MSG_21922));
    break;
  case L_COMMANDANT:
    if (signp) {
      print1(LS(IDS_MSG_21908));
      print2(LS(IDS_MSG_21923));
    }
    break;
  case L_DINER:
    if (signp) {
      print1(LS(IDS_MSG_21908));
      print2(LS(IDS_MSG_21924));
    }
    break;
  case L_CRAP:
    if (signp) {
      print1(LS(IDS_MSG_21908)); 
      print2(LS(IDS_MSG_21925));
    }
    break;
  case L_TAVERN:
    if (signp) {
      print1(LS(IDS_MSG_21908)); 
      print2(LS(IDS_MSG_21926));
    }
    break;
  case L_ALCHEMIST:
    if (signp) {
      print1(LS(IDS_MSG_21908)); 
      print2(LS(IDS_MSG_21927));
    }
    break;
  case L_DPW:
    if (signp) {
      print1(LS(IDS_MSG_21908)); 
      print2(LS(IDS_MSG_21928));
    }
    break;
  case L_LIBRARY:
    if (signp) {
      print1(LS(IDS_MSG_21908)); 
      print2(LS(IDS_MSG_21929));
    }
    break;
  case L_CONDO:
    if (signp) {
      print1(LS(IDS_MSG_21908)); 
      if (gamestatusp(SOLD_CONDO)) 
	print2(LS(IDS_MSG_21930));
      else print2(LS(IDS_MSG_21931));
    }
    break;
  case L_PAWN_SHOP:
    if (signp) {
      print1(LS(IDS_MSG_21908)); 
      print2(LS(IDS_MSG_21932));
    }
    break;
  case L_CEMETARY:
    print1(LS(IDS_MSG_21908)); 
    print2(LS(IDS_MSG_21933));
    break;
  case L_GARDEN:
    print1(LS(IDS_MSG_21908)); 
    print2(LS(IDS_MSG_21934));
    break;
  case L_JAIL:
    print1(LS(IDS_MSG_21908)); 
    print2(LS(IDS_MSG_21935));
    break;
  case L_ORACLE:
    print1(LS(IDS_MSG_21908));
    print2(LS(IDS_MSG_21936));
    morewait();
    break;
  }
}
#endif
  


void l_countryside()
{
  change_environment(E_COUNTRYSIDE);
}



void l_oracle()
{
  char response;
  if (gamestatusp(ATTACKED_ORACLE) && (! gamestatusp(COMPLETED_ASTRAL))) {
    print1(LS(IDS_MSG_22505));
    print2(LS(IDS_MSG_22506));
    if (ynq2()=='y') {
      print1(LS(IDS_MSG_22507));
      Player.hp = 1;
      print2(LS(IDS_MSG_22508));
      dataprint();
      print3(LS(IDS_MSG_22509));
      change_environment(E_ASTRAL);
    }
  }
  else {
    print1(LS(IDS_MSG_22510));
    print2(LS(IDS_MSG_22511));
    do response = (char) mcigetc();
    while ((response != 'b') && (response != 'm') && (response != ESCAPE));
    if (response == 'b') {
      print1(LS(IDS_MSG_22512));
      print2(LS(IDS_MSG_22513));
      morewait();
      print1(LS(IDS_MSG_22514));
      print2(LS(IDS_MSG_22515));
      if (ynq2() == 'y') {
	setgamestatus(ATTACKED_ORACLE);
	print1(LS(IDS_MSG_22516));
	print2(LS(IDS_MSG_22517));
      }
      else {
	print2(LS(IDS_MSG_22518));
	if (!gamestatusp(SPOKE_TO_DRUID)) {
	  print3(LS(IDS_MSG_22519));
	}
	else if (!gamestatusp(COMPLETED_CAVES)) {
	  print3(LS(IDS_MSG_22520));
	}
	else if (!gamestatusp(COMPLETED_SEWERS)) {
	  print3(LS(IDS_MSG_22521));
	}
	else if (!gamestatusp(COMPLETED_CASTLE)) {
	  print3(LS(IDS_MSG_22522));
	}
	else if (!gamestatusp(COMPLETED_ASTRAL)) {
	  morewait();
	  print1(LS(IDS_MSG_22523));
	  print2(LS(IDS_MSG_22524));
	  if (ynq2()=='y') {
	    print1(LS(IDS_MSG_22525));
	    print2(LS(IDS_MSG_22526));
	    morewait();
	    print1(LS(IDS_MSG_22527));
	    print2(LS(IDS_MSG_22528));
	    morewait();
	    clearmsg();
	    print1(LS(IDS_MSG_22529));
	    gain_experience(5000);
	    change_environment(E_ASTRAL);
	  }
	  else print3(LS(IDS_MSG_22530));
	}
	else if (!gamestatusp(COMPLETED_VOLCANO)) {
	  print3(LS(IDS_MSG_22531));
	}
	else if (!gamestatusp(COMPLETED_CHALLENGE)) {
	  print3(LS(IDS_MSG_22532));
	}
	else {
	  morewait();
	  print1(LS(IDS_MSG_22533));
	  print2(LS(IDS_MSG_22534));
	  print3(LS(IDS_MSG_22535));
	}
      }
    }
    else if (response == 'm') {
      print1(LS(IDS_MSG_22536));
      knowledge(1);
    }
    else print2(LS(IDS_MSG_22537));
  }
}

void l_mansion()
{
  print1(LS(IDS_MSG_22538));
  if (ynq1()=='y')
    change_environment(E_MANSION);
}

void l_house()
{
  print1(LS(IDS_MSG_22539));
  if (ynq1()=='y')
    change_environment(E_HOUSE);
}

void l_hovel()
{
  print1(LS(IDS_MSG_22540));
  if (ynq1()=='y')
    change_environment(E_HOVEL);
}

void l_safe()
{
  char response;
  pob newitem;
  int attempt = 0;
  print1(LS(IDS_MSG_22541));
  print2(LS(IDS_MSG_22542));
  do response = (char) mcigetc();
  while ((response != 'p') && (response != 'f') && (response != ESCAPE));
  if (response == 'p')
    attempt = (2*Player.dex + Player.rank[THIEVES]*10 - random_range(100))/10;
  else if (response == 'f')
    attempt = (Player.dmg - random_range(100))/10;
  if (attempt > 0) {
    Player.alignment -= 4;
    gain_experience(50);
    print2(LS(IDS_MSG_22543));
    Level->site[Player.x][Player.y].locchar = FLOOR;
    Level->site[Player.x][Player.y].p_locf = L_NO_OP;
    lset(Player.x, Player.y, CHANGED);
    if (random_range(2) == 1) {
      print1(LS(IDS_MSG_22544));
      do {
	newitem = NULL;
	while (newitem == NULL)
	  newitem = create_object(difficulty());
	print2(itemid(newitem));
	morewait();
	gain_item(newitem);
      } while (random_range(3) == 1);
    }
    else print2(LS(IDS_MSG_22545));
  }
  else {
    print3(LS(IDS_MSG_22546));
    if (attempt == -1) {
      print1(LS(IDS_MSG_22547));
      if (Last_Environment == E_CITY) {
	print2(LS(IDS_MSG_22548));
	change_environment(E_CITY);
	morewait();
	send_to_jail();
      }
    }
    else if (attempt == -2) {
      print1(LS(IDS_MSG_22549));
      p_damage(random_range(25),FLAME,"a safe");
      print2(LS(IDS_MSG_22550));
      Level->site[Player.x][Player.y].locchar = RUBBLE;
      Level->site[Player.x][Player.y].p_locf = L_RUBBLE;
      lset(Player.x, Player.y, CHANGED);
    }
    else if (attempt == -3) {
      print1(LS(IDS_MSG_22551));
      lball(Player.x,Player.y,Player.x,Player.y,30);
    }
    else if (attempt < -3) {
      print1(LS(IDS_MSG_22552));
      if (Player.possessions[O_CLOAK] != NULL) {
	print2(LS(IDS_MSG_22553));
	conform_lost_object(Player.possessions[O_CLOAK]);
	p_damage(10,ACID,"a safe");
      }
      else if (Player.possessions[O_ARMOR] != NULL) {
	print2(LS(IDS_MSG_22554));
	Player.possessions[O_ARMOR]->dmg-=3;
	Player.possessions[O_ARMOR]->hit-=3;
	Player.possessions[O_ARMOR]->aux-=3;
	p_damage(10,ACID,"a safe");
      }
      else {
	print2(LS(IDS_MSG_22555));
	p_damage(random_range(100),ACID,"a safe");
      }
    }
  }
}


void l_cartographer()
{
  int i,j,x,y;
  print1(LS(IDS_MSG_22556));
  print2(LS(IDS_MSG_22557));
  if (ynq2()=='y') {
    if (Player.cash < 500) 
      print3(LS(IDS_MSG_22558));
    else {
      print3(LS(IDS_MSG_22559));
      Player.cash -= 500;
      dataprint();
      switch(Villagenum) {
      case 1: x = 56; y = 5; break;
      default: case 2: x = 35; y = 11; break;
      case 3: x = 10; y = 40; break;
      case 4: x = 7; y = 6; break;
      case 5: x = 40; y = 43; break;
      case 6: x = 20; y = 41; break;
      }	
      for(i=x-15;i<=x+15;i++)
	for(j=y-15;j<=y+15;j++)
	  if ((i>=0)&&(i<64)&&(j>=0)&&(j<64)) {
	    if (Country[i][j].current_terrain_type !=
	      Country[i][j].base_terrain_type) {
	      c_set(i, j, CHANGED);
	      Country[i][j].current_terrain_type =
		Country[i][j].base_terrain_type;
	    }
	    c_set(i, j, SEEN);
	  }
    }
  }
  else print3(LS(IDS_MSG_22560));
}
	    
void l_charity()
{
  long donation;
  print2(LS(IDS_MSG_22561));
  if (ynq2()!='y') 
    print3(LS(IDS_MSG_22562));
  else {
    clearmsg();
    print1(LS(IDS_MSG_22563));
    donation = parsenum();
    if (donation < 1)
      print2(LS(IDS_MSG_22564));
    else if (donation > Player.cash)
      print2(LS(IDS_MSG_22565));
    else if (donation < max(100,Player.level*Player.level*100)) {
      print2(LS(IDS_MSG_22566));
      print3(LS(IDS_MSG_22567));
      if (Player.alignment < 10) Player.alignment++;
      Player.cash -= donation;
    }
    else {
      print2(LS(IDS_MSG_22568));
      Player.cash -= donation;
      Player.alignment += 5;
    }
  }
  dataprint();
} 

