/* omega copyright (C) by Laurence Raphael Brothers, 1987,1988,1989 */
/* site1.c */
/* 1st half of site functions and aux functions to them */

#if !defined(MSDOS) && !defined(_WIN32)
#include <unistd.h>
#endif

#include "glob.h"

/* the bank; can be broken into (!) */
void l_bank()
{
  int done=FALSE,valid=FALSE;
  long amount;
  char response;
  char passwd[64];
  print1(LS(IDS_MSG_21915));

  if (gamestatusp(BANK_BROKEN))
    print2(LS(IDS_MSG_22221));
  else {
    print2(LS(IDS_MSG_22222));
    morewait();
    while (! done) {
      clearmsg();
      print1(LS(IDS_MSG_22223));
      mlongprint(Balance);
      nprint1(LS(IDS_MSG_22224));
      nprint1(LS(IDS_MSG_22225));
      response = mgetc();
      if (response == '?') {
	menuclear();
	menuprint("?: This List.\n");
	if (strcmp(Password,"")==0)
	  menuprint("O: Open an account.\n");
	else {
	  menuprint("P: Enter password.\n");
	  menuprint("D: Deposit.\n");
	  menuprint("W: Withdraw\n");
	}
	menuprint("X: eXit\n");
	showmenu();
	morewait();
	xredraw();
	continue;
      }
      else if ((response == 'P') && (strcmp(Password,"") != 0)) {
	clearmsg();
	print1(LS(IDS_MSG_22226));
	strcpy(passwd,msgscanstring());
	valid = (strcmp(passwd,Password)==0);
	if (! valid) {
	  done = TRUE;
	  menuclear();
	  menuprint("Alert! Alert! Invalid Password!\n");
	  menuprint("The police are being summoned!\n");
	  menuprint("Please wait for the police to arrive....\n\n");
	  menuprint("----Hit space bar to continue----\n");
	  showmenu();
	  response = menugetc();
	  if (response == ' ') {
	    Player.alignment += 5;
	    xredraw();
	    print1(LS(IDS_MSG_22227));
	    print2(LS(IDS_MSG_22228));
	    morewait();
	    send_to_jail();
	  }
	  else {
	    Player.alignment -= 5;
	    menuclear();
	    sleep(4);
	    menuprint("^@^@^@^@^@00AD1203BC0F0000FFFFFFFFFFFF\n");
	    menuprint("Interrupt in _get_space. Illegal Character.\n");
	    showmenu();
	    sleep(4);
	    menuprint("Aborting _police_alert.....\n");
	    menuprint("Attempting reboot.....\n");
	    showmenu();
	    sleep(4);
	    menuprint("Warning: Illegal shmop at _count_cash.\n");
	    menuprint("Warning: Command Buffer NOT CLEARED\n");
	    showmenu();
	    sleep(4);
	    menuprint("Reboot Complete. Execution Continuing.\n");
	    menuprint("Withdrawing: 4294967297 Au.\n");
	    menuprint("Warning: Arithmetic Overflow in _withdraw\n");
	    showmenu();
	    sleep(4);
	    menuprint("Yo mama. Core dumped.\n");
	    showmenu();
	    sleep(4);
	    xredraw();
	    clearmsg();
	    print1(LS(IDS_MSG_22229));
	    print2(LS(IDS_MSG_22230));
	    Player.cash += Balance + 1000 + random_range(3000);
	    Balance = 0;
	    setgamestatus(BANK_BROKEN);
	  }
	}
	else print2(LS(IDS_MSG_22231));
      }
      else if ((response == 'D') && valid) {
	clearmsg();
	print1(LS(IDS_MSG_22232));
	amount = get_money(Player.cash);
	if (amount < 1) 
	  print3(LS(IDS_MSG_22233));
	else if (amount > Player.cash)
	  print3(LS(IDS_MSG_22234));
	else {
	  print2(LS(IDS_MSG_22235));
	  Balance += amount;
	  Player.cash -= amount;
	}
      }
      else if ((response == 'W') && valid) {
	clearmsg();
	print2(LS(IDS_MSG_22232));
	amount = get_money(Balance);
	if (amount < 1) 
	  print3(LS(IDS_MSG_22233));
	else if (amount > Balance) 
	  print3(LS(IDS_MSG_22236));
	else {
	  print2(LS(IDS_MSG_22235));
	  Balance -= amount;
	  Player.cash += amount;
	}
      }
      else if (response == 'X') {
	clearmsg();
	print1(LS(IDS_MSG_20507));
	done = TRUE;
      }
      else if ((response == 'O') && (strcmp(Password,"")==0)) {
	clearmsg();
	print1(LS(IDS_MSG_22237));
	nprint1(LS(IDS_MSG_22238));
	strcpy(Password,msgscanstring());
	if (strcmp(Password,"")==0) {
	  print3(LS(IDS_MSG_22239));
	  done = TRUE;
	}
	else {
	  print2(LS(IDS_MSG_22240));
	  valid = TRUE;
	}
      }
      else print3(LS(IDS_MSG_22241));
      dataprint();
      morewait();
    }
  }
  xredraw();
}




void l_armorer()
{
  int done = FALSE;
  char action;
  if (hour() == 12) 
    print3(LS(IDS_MSG_22242));
  else if (nighttime()) 
    print3(LS(IDS_MSG_22243));
  else {
    while (! done) {
      clearmsg();
      print1(LS(IDS_MSG_22244));
      action = mgetc();
      if (action == ESCAPE) 
	done = TRUE;
      else if (action == 'a') 
	buyfromstock(ARMORID,10);
      else if (action == 'w') 
	buyfromstock(WEAPONID,23);
    }
  }
  xredraw();
}


void buyfromstock(base,numitems)
int base,numitems;  
{
  int i;
  char item;
  pob newitem;

  print2(LS(IDS_MSG_22245));
  menuclear();
  for(i=0;i<numitems;i++) {
    strcpy(Str4,LS(IDS_MSG_23191));
    Str4[0] = i + 'a';
    strcat(Str4,Objects[base+i].objstr);
    menuprint(Str4);
    menuprint("\n");
  }
  showmenu();
  item = ' ';
  while ((item != ESCAPE) &&
	 ((item < 'a') || (item >= 'a'+numitems)))
    item = mgetc();
  if (item != ESCAPE) {
    i = item - 'a';
    newitem = ((pob) checkmalloc(sizeof(objtype)));
    *newitem = Objects[base+i];
    newitem->known = 2;
    clearmsg();
    print1(LS(IDS_MSG_22246));
    mlongprint(2*true_item_value(newitem));
    nprint1(LS(IDS_MSG_22247));
    if (ynq1() == 'y') {
      if (Player.cash < 2*true_item_value(newitem)) {
	print2(LS(IDS_MSG_22248));
	free((char *) newitem);
      }
      else {
	Player.cash -= 2*true_item_value(newitem);
	dataprint();
	gain_item(newitem);
      }
    }
    else free((char *)newitem);
  }
}


void l_club()
{
#define hinthour club_hinthour
  char response;

  print1(LS(IDS_MSG_21918));
  if (! gamestatusp(CLUB_MEMBER)) {
    if (Player.level < 2) print3(LS(IDS_MSG_22249));
    else {
      print2(LS(IDS_MSG_22250));
      if (ynq2()=='y') {
	if (Player.cash < 100)
	  print3(LS(IDS_MSG_22251));
	else {
	  print1(LS(IDS_MSG_22252));
          print2(LS(IDS_MSG_22253));
	  morewait();
	  clearmsg();
	  print1(LS(IDS_MSG_22254));
	  print2(LS(IDS_MSG_22255));
	  Spells[S_RETURN].known = TRUE;
	  Player.cash -= 100;
	  setgamestatus(CLUB_MEMBER);
	}
      }
      else print2(LS(IDS_MSG_22256));
    }
  }
  else {
    print2(LS(IDS_MSG_22257));
    do response = (char) mcigetc(); 
    while ((response != 's') && (response != 'l') && (response != ESCAPE));
    if (response == 'l') {
      if (hinthour == hour()) print2(LS(IDS_MSG_22258));
      else {
	print1(LS(IDS_MSG_22259));
	hint();
	hinthour = hour();
      }
    }
    else if (response == 's') {
      buyfromstock(THINGID+7,2);
      xredraw();
    }
    else if (response == ESCAPE)
      print2(LS(IDS_MSG_22260));
  }
}
#undef hinthour

void l_gym()
{
  int done=TRUE;
  clearmsg();
  do {
    print1(LS(IDS_MSG_22261));
    if ((Gymcredit > 0) || (Player.rank[ARENA])) {
      nprint1(LS(IDS_MSG_22262));
      mlongprint(Gymcredit);
      nprint1(LS(IDS_MSG_21143));
    }
    done = FALSE;
    menuclear();
    menuprint("Train for 2000 Au. Choose:\n");
    menuprint("\na: work out in the weight room");
    menuprint("\nb: use our gymnastics equipment");
    menuprint("\nc: take our new anaerobics course");
    menuprint("\nd: enroll in dance lessons.");
    menuprint("\nESCAPE: Leave this place.");
    showmenu();
    switch(mgetc()) {
    case 'a': 
      gymtrain(&(Player.maxstr),&(Player.str));
      break;
    case 'b': 
      gymtrain(&(Player.maxdex),&(Player.dex));
      break;
    case 'c': 
      gymtrain(&(Player.maxcon),&(Player.con));
      break;
    case 'd':
      gymtrain(&(Player.maxagi),&(Player.agi));
      break;
    case ESCAPE:
      clearmsg();
      print1(LS(IDS_MSG_22263)); 
      done = TRUE;
      break;
    }
  } while (! done);
  xredraw();
  calc_melee();
}



void l_healer()
{
  print1(LS(IDS_MSG_21920));
  morewait();
  clearmsg();
  print1(LS(IDS_MSG_22264));
  print2(LS(IDS_MSG_22265));
  print3(LS(IDS_MSG_22266));
  switch((char) mcigetc()) {
    case 'a': healforpay(); break;
    case 'b': cureforpay(); break;
    default: print3(LS(IDS_MSG_22267)); break;
  }	      
}


void statue_random(x,y)
int x,y;
{
  pob item;
  int i,j;
  switch(random_range(difficulty()+3)-1) {
  default: l_statue_wake(); break;
  case 0: 
    print1(LS(IDS_MSG_22268));
    Level->site[x][y].locchar = RUBBLE;
    Level->site[x][y].p_locf = L_RUBBLE;
    plotspot(x, y, TRUE);
    lset(x, y, CHANGED);
    break;
  case 1: 
    print1(LS(IDS_MSG_22269));
    break;
  case 2: 
    print1(LS(IDS_MSG_22268));
    Level->site[x][y].locchar = RUBBLE;
    Level->site[x][y].p_locf = L_RUBBLE;
    plotspot(x, y, TRUE);
    lset(x, y, CHANGED);
    make_site_treasure(x,y,difficulty());
    break;
  case 3:
    print1(LS(IDS_MSG_22270));
    p_damage(random_range(difficulty()*5),UNSTOPPABLE,"a statue");
    break;
  case 4:
    print1(LS(IDS_MSG_22271));
    morewait();
    clearmsg();
    hint();
    break;
  case 5:
    if ((Current_Environment == Current_Dungeon) ||
	(Current_Environment == E_CITY)) {
      print1(LS(IDS_MSG_22272));
      print2(LS(IDS_MSG_22273));
      Level->site[x][y].locchar = STAIRS_DOWN;
      Level->site[x][y].p_locf = L_NO_OP;
      lset(x, y, CHANGED);
    }
    break;
  case 6:
    print1(LS(IDS_MSG_22274));
    print2(LS(IDS_MSG_22275));
    Player.status[IMMOBILE]+=random_range(6)+2;
    break;
  case 7:
    print1(LS(IDS_MSG_22276));
    dispel(-1);
    break;
  case 8: /* I think this is particularly evil. Heh heh. */
    if (Player.possessions[O_WEAPON_HAND] != NULL) {
      print1(LS(IDS_MSG_22277));
      item = Player.possessions[O_WEAPON_HAND];
      conform_lost_object(Player.possessions[O_WEAPON_HAND]);
      item->blessing = -1-abs(item->blessing);
      drop_at(x,y,item);
    }
    break;
  case 9:
    print1(LS(IDS_MSG_22278));
    for(i=0;i<WIDTH;i++)
      for(j=0;j<LENGTH;j++) {
	lset(i,j,SEEN);
	if (loc_statusp(i,j,SECRET)) {
	  lreset(i,j,SECRET);
	  lset(i,j,CHANGED);
	}
      }
    show_screen();
    break;
  }
}

void l_statue_wake()
{
  int i;
  int x=Player.x,y=Player.y;
  for(i=0;i<9;i++)
    wake_statue(x+Dirs[0][i],y+Dirs[1][i],TRUE);
}

void wake_statue(x,y,first)
int x,y,first;
{
  int i;
  pml tml;
  if (Level->site[x][y].locchar == STATUE) {
    if (! first) mprint(LS(IDS_MSG_22279));
    else mprint(LS(IDS_MSG_22280));
    Level->site[x][y].locchar = FLOOR;
    lset(x, y, CHANGED);
    tml = ((pml) checkmalloc(sizeof(mltype)));
    tml->m =
      (Level->site[x][y].creature = m_create(x,y,0,difficulty()+1));
    m_status_set(Level->site[x][y].creature,HOSTILE);
    tml->next = Level->mlist;
    Level->mlist = tml;
    for(i=0;i<8;i++) wake_statue(x+Dirs[0][i],y+Dirs[1][i],FALSE);
  }
}


void l_casino()
{
  int i,done = FALSE,a,b,c,match;
  char response;
  print1(LS(IDS_MSG_21921));
  if (random_range(10)==1)
    print2(LS(IDS_MSG_22281));
  else {
    while (! done) {
      morewait();
      clearmsg();
      print1(LS(IDS_MSG_22282));
      print2(LS(IDS_MSG_22283));
      print3(LS(IDS_MSG_22284));
      response = (char) mcigetc();
      if (response == 'a') {
	if (Player.cash < 100) print3(LS(IDS_MSG_22285));
	else {
	  Player.cash -= 100;
	  dataprint();
	  for(i=0;i<20;i++) {
	    if (i==19)
	      sleep(1);
	    else
	      usleep(250000);
	    a = random_range(10);
	    b = random_range(10);
	    c = random_range(10);
	    clearmsg1();
	    mprint(slotstr(a));
	    mprint(slotstr(b));
	    mprint(slotstr(c));
	  }
	  if (winnings > 0) do {
	    a = random_range(10);
	    b = random_range(10);
	    c = random_range(10);
	  } while ((a==b) || (a == c) || (b == c));
	  else {
	    a = random_range(10);
	    b = random_range(10);
	    c = random_range(10);
	  }
	  clearmsg();
	  mprint(slotstr(a));
	  mprint(slotstr(b));
	  mprint(slotstr(c));
	  if ((a==b) && (a==c)) {
	    print3(LS(IDS_MSG_22286));
	    winnings += (a+2)*(b+2)*(c+2)*5;
	    Player.cash += (a+2)*(b+2)*(c+2)*5;
	    dataprint();
	  }
	  else if (a==b) {
	    print3(LS(IDS_MSG_22287));
	    Player.cash += (a+2)*(b+2)*5;
	    dataprint();
	    winnings += (a+2)*(b+2)*5;
	  }
	  else if (a==c) {
	    print3(LS(IDS_MSG_22287));
	    Player.cash += (a+2)*(c+2)*5;
	    dataprint();
	    winnings += (a+2)*(c+2)*5;
	  }
	  else if (c==b) {
	    print3(LS(IDS_MSG_22287));
	    Player.cash += (c+2)*(b+2)*5;
	    dataprint();
	    winnings += (c+2)*(b+2)*5;
	  }
	  else {
	    print3(LS(IDS_MSG_22288));
	    winnings -= 100;
	  }
	}
      }
      else if (response == 'b') {
	if (Player.cash < 1000) mprint(LS(IDS_MSG_22285));
	else {
	  Player.cash -= 1000;
	  dataprint();
	  print1(LS(IDS_MSG_22289));
	  do response = (char) mcigetc();
	  while ((response != 'r') && (response != 'b'));
	  match = (response == 'r' ? 0 : 1);
	  for(i=0;i<20;i++) {
	    if (i==19)
	      sleep(1);
	    else
	      usleep(250000);
	    a = random_range(37);
	    b = a % 2;
	    if (a == 0) print1(LS(IDS_MSG_22290));
	    else if (a==1) print1(LS(IDS_MSG_22291));
	    else {
	      print1(( b == 0) ? "Red ": "Black ");
	      mnumprint(a-1);
	    }
	  }
	  if (winnings > 0) do {
	    a = random_range(37);
	    b = a % 2;
	  } while (b == match);
	  else {
	    a = random_range(37);
	    b = a % 2;
	  }
	  if (a == 0) print1(LS(IDS_MSG_22290));
	  else if (a==1) print1(LS(IDS_MSG_22291));
	  else {
	    print1((b == 0) ? "Red ": "Black ");
	    mnumprint(a-1);
	  }
	  if ((a > 1) && (b == match)){
	    print3(LS(IDS_MSG_22292));
	    winnings += 1000;
	    Player.cash += 2000;
	    dataprint();
	  }
	  else {
	    print3(LS(IDS_MSG_22293));
	    winnings -= 1000;
	    dataprint();
	  }
	}
      }
      else if (response == ESCAPE) done = TRUE;
    }
  }
}





void l_commandant()
{
  int num;
  pob food;
  print1(LS(IDS_MSG_21923));
  print2(LS(IDS_MSG_22294));
  if (ynq2()=='y') {
    clearmsg();
    print1(LS(IDS_MSG_22295));
    num = (int) parsenum();
    if (num < 1) print3(LS(IDS_MSG_22296));
    else if (num*5 > Player.cash)
      print3(LS(IDS_MSG_22297));
    else {
      Player.cash -= num*5;
      food = ((pob) checkmalloc(sizeof(objtype)));
      *food = Objects[FOODID+0]; /* food ration */
      food->number = num;
      if (num == 1)
	print2(LS(IDS_MSG_22298));
      else print2(LS(IDS_MSG_22299));
      morewait();
      gain_item(food);
    }
  }
  else print2(LS(IDS_MSG_22300));
}


void l_diner()
{
  print1(LS(IDS_MSG_21924));
  print2(LS(IDS_MSG_22301));
  if (ynq2()=='y') {
    if (Player.cash < 25)
      mprint(LS(IDS_MSG_22302));
    else {
      Player.cash -= 25;
      dataprint();
      Player.food = 44;
      foodcheck();
    }
  }
}

void l_crap()
{
  print1(LS(IDS_MSG_22303));
  if ((hour() < 17) || (hour() > 23))
    print2(LS(IDS_MSG_22304));
  else {
    print2(LS(IDS_MSG_22305));
    if (ynq2()=='y') {
      if (Player.cash < 1000)
	print2(LS(IDS_MSG_22306));
      else {
	print2(LS(IDS_MSG_22307));
	Player.cash -= 1000;
	dataprint();
	Player.food += 8;
	foodcheck();
      }
    }
  }
}

void l_tavern()
{
#define hinthour tavern_hinthour
  char response;
  print1(LS(IDS_MSG_21926));
  if (nighttime()) {
    menuclear();
    menuprint("Riley says: Whataya have?\n\n");
    menuprint("a: Pint of Riley's ultra-dark 1Au\n");
    menuprint("b: Shot of Tullimore Dew 10Au\n");
    menuprint("c: Round for the House. 100Au\n");
    menuprint("d: Bed and Breakfast. 25Au\n");
    menuprint("ESCAPE: Leave this comfortable haven.\n");
    showmenu();
    do response = (char) mcigetc();
    while ((response != 'a') &&
	   (response != 'b') &&
	   (response != 'c') &&
	   (response != 'd') &&
	   (response != ESCAPE));
    switch (response) {
    case 'a':
      if (Player.cash < 1)
	print2(LS(IDS_MSG_22308));
      else {
	hinthour = hour();
	Player.cash -= 1;
	dataprint();
	if (hinthour!=hour()) {
	  if (random_range(3)) {
	    print1(LS(IDS_MSG_22309));
	    hint();
	  }
	  else print1(LS(IDS_MSG_22310));
	  hinthour = hour();
	}
	else print1(LS(IDS_MSG_22311));
      }
      break;
    case 'b':
      if (Player.cash < 10)
	print2(LS(IDS_MSG_22312));
      else {
	Player.cash -= 10;
	print1(LS(IDS_MSG_22313));
	if (Player.status[POISONED] || Player.status[DISEASED])
	  print2(LS(IDS_MSG_22314));
	Player.status[POISONED] = 0;
	Player.status[DISEASED] = 0;
	showflags();
      }
      break;
    case 'c':
      if (Player.cash < 100) {
	print1(LS(IDS_MSG_22315));
	print2(LS(IDS_MSG_22316));
	p_damage(random_range(20),UNSTOPPABLE,"Riley's right cross");
	morewait();
      }
      else {
	Player.cash -= 100;
	dataprint();
	print1(LS(IDS_MSG_22317)); morewait();
	print2(LS(IDS_MSG_22318)); morewait();
	print3(LS(IDS_MSG_22319)); morewait();
	clearmsg();
	switch(random_range(4)) {
	case 0:
	  print1(LS(IDS_MSG_22320));
	  hint();
	  break;
	case 1:
	  print1(LS(IDS_MSG_22321));
	  if ((Player.patron == ODIN) || (Player.patron == ATHENA))
	    Player.alignment++;
	  else if ((Player.patron == HECATE) || (Player.patron == SET))
	    Player.alignment--;
	  else if (Player.alignment > 0) Player.alignment--;
	  else Player.alignment++;
	  break;
	case 2:
	  print1(LS(IDS_MSG_22322));
	  gain_experience(20);
	  break;
	case 3:
	  print1(LS(IDS_MSG_22323));
	  print2(LS(IDS_MSG_22324));
	  if (ynq2()=='y') {
	    if (Player.con < random_range(20)) {
	      print1(LS(IDS_MSG_22325));
	      print2(LS(IDS_MSG_22326));
	      Player.con++;
	      Player.maxcon++;
	    }
	    else print2(LS(IDS_MSG_22327));
	  }
	}		
      }
      break;
    case 'd':
      if (Player.cash < 25)
	print2(LS(IDS_MSG_22328));
      else {
	Player.cash -= 25;
	print2(LS(IDS_MSG_22329));
	morewait();
	Time += (6+random_range(4)) * 60;
	Player.status[POISONED] = 0;
	Player.status[DISEASED] = 0;
	Player.food = 40;
	/* reduce temporary stat gains to max stat levels */
	toggle_item_use(TRUE);
	Player.str = min(Player.str,Player.maxstr);
	Player.con = min(Player.con,Player.maxcon);
	Player.agi = min(Player.agi,Player.maxagi);
	Player.dex = min(Player.dex,Player.maxdex);
	Player.iq = min(Player.iq,Player.maxiq);
	Player.pow = min(Player.pow,Player.maxpow);
	toggle_item_use(FALSE);
	timeprint();
	dataprint();
	showflags();
	print1(LS(IDS_MSG_22330));
	if (hour() > 10) print2(LS(IDS_MSG_22331));
      }
      break;
    default:
      print2(LS(IDS_MSG_22332));
      break;
    }
  }
  else print2(LS(IDS_MSG_22333));
  xredraw();
}
#undef hinthour


void l_alchemist()
{
  int i,done=FALSE,mlevel;
  char response;
  pob obj;
  print1(LS(IDS_MSG_21927));
  if (nighttime()) 
    print2(LS(IDS_MSG_22334));
  else while (! done){
    morewait();
    clearmsg();
    print1(LS(IDS_MSG_22335));
    print2(LS(IDS_MSG_22336));
    print3(LS(IDS_MSG_22337));
    response = (char) mcigetc();
    if (response == 'a') {
      clearmsg();
      done = TRUE;
      i = getitem(CORPSE);
      if ((i != ABORT) && (Player.possessions[i] != NULL)){
	obj = Player.possessions[i];
	if (Monsters[obj->charge].transformid == -1) {
	  print1(LS(IDS_MSG_22338));
	  if (obj->basevalue > 0)
	    print2(LS(IDS_MSG_22339));
	}
	else {
	  clearmsg();
	  print1(LS(IDS_MSG_21186));
	  mnumprint(obj->basevalue/3);
	  nprint1(LS(IDS_MSG_22340));
	  if (ynq1()=='y') {
	    Player.cash += (obj->basevalue/3);
	    conform_lost_objects(1,obj);
	  }
	  else print2(LS(IDS_MSG_22341));
	}
      }
      else print2(LS(IDS_MSG_22342));
    }
    else if (response == 'b') {
      clearmsg();
      done = TRUE;
      i = getitem(CORPSE);
      if ((i != ABORT) && (Player.possessions[i] != NULL)){
	obj = Player.possessions[i];
	if (Monsters[obj->charge].transformid == -1)
	  print1(LS(IDS_MSG_22343));
	else {
	  mlevel = Monsters[obj->charge].level;
	  print1(LS(IDS_MSG_22344));
	  mnumprint(max(10,obj->basevalue*2));
	  nprint1(LS(IDS_MSG_22345));
	  if (ynq1()=='y') {
	    if (Player.cash < max(10,obj->basevalue*2))
	      print2(LS(IDS_MSG_22346));
	    else {
	      print1(LS(IDS_MSG_22347));
	      Player.cash -= max(10,obj->basevalue*2);
	      *obj = Objects[Monsters[obj->charge].transformid];
	      if ((obj->id >= STICKID) && (obj->id < STICKID+NUMSTICKS))
		obj->charge = 20;
	      if (obj->plus == 0) obj->plus = mlevel;
	      if (obj->blessing == 0) obj->blessing = 1;
	    }
	  }
	  else print2(LS(IDS_MSG_22348));
	}
      }
      else print2(LS(IDS_MSG_22342));
    }
    else if (response == ESCAPE) done = TRUE;
  }
}

void l_dpw()
{
  print1(LS(IDS_MSG_21928));
  if (Date - LastDay < 7)
    print2(LS(IDS_MSG_22349));
  else if (Player.cash < 100) {
    print2(LS(IDS_MSG_22350));
    if (ynq2()=='y') {
      print1(LS(IDS_MSG_22351));
      print2(LS(IDS_MSG_22352));
      if (strcmp(Player.name,msgscanstring()) != 0) {
	print3(LS(IDS_MSG_22353));
	morewait();
	send_to_jail();
      }
      else {
	print1(LS(IDS_MSG_22354));
	LastDay = Date;
	Player.cash = 99;
	dataprint();
      }
    }
  }
  else print2(LS(IDS_MSG_22355));
}

void l_library()
{
  char response;
  int done=FALSE,fee = 1000;
  print1(LS(IDS_MSG_21929));
  if (nighttime())
    print2(LS(IDS_MSG_22356));
  else {
    morewait();
    print1(LS(IDS_MSG_22357));
    if (Player.maxiq < 18) {
      print2(LS(IDS_MSG_22358));
      morewait();
      clearmsg();
      print1(LS(IDS_MSG_22359));
      mnumprint(fee=max(50,1000-(18-Player.maxiq)*125));
      nprint1(LS(IDS_MSG_21143));
    }
    morewait();
    while(! done) {
      print1(LS(IDS_MSG_22360));
      if (ynq1()=='y') {
	if (Player.cash < fee) {
	  print2(LS(IDS_MSG_22361));
	  done = TRUE;
	}
	else {
	  Player.cash -= fee;
	  dataprint();
	  menuclear();
	  menuprint("Peruse a scroll:\n");
	  menuprint("a: Omegan Theology\n");
	  menuprint("b: Guide to Rampart\n");
	  menuprint("c: High Magick\n");
	  menuprint("d: Odd Uncatalogued Document\n");
	  menuprint("e: Attempt Advanced Research\n");
	  menuprint("ESCAPE: Leave this font of learning.\n");
	  showmenu();
	  response = (char) mcigetc();
	  if (response == 'a') {
	    print1(LS(IDS_MSG_22362));
	    morewait();
	    theologyfile();
	  }
	  else if (response == 'b') {
	    print1(LS(IDS_MSG_22363));
	    morewait();
	    cityguidefile();
	  }
	  else if (response == 'c') {
	    print1(LS(IDS_MSG_22364));
	    morewait();
	    wishfile();
	  }
	  else if (response == 'd') {
	    print1(LS(IDS_MSG_22365));
	    print2(LS(IDS_MSG_22366));
	    morewait();
	    adeptfile();
	  }
	  else if (response == 'e') {
	    if (random_range(30) > Player.iq)  {
	      print2(LS(IDS_MSG_22367));
	      Player.iq++;
	      Player.maxiq++;
	      dataprint();
	      if (Player.maxiq < 19 &&
		fee != max(50,1000-(18-Player.maxiq)*125))
		{
		  morewait();
		  clearmsg();
		  print1(LS(IDS_MSG_22359));
		  mnumprint(fee=max(50,1000-(18-Player.maxiq)*125));
		  nprint1(LS(IDS_MSG_21143));
		  morewait();
		}
	    }
	    else {
	      clearmsg1();
	      print1(LS(IDS_MSG_22368));
	      morewait();
	      hint();
	      morewait();
	    }
	  }
	  else done = (response == ESCAPE);
	}
      xredraw();
      }
      else {
	done = TRUE;
	print2(LS(IDS_MSG_22369));
      }
    }
  }
}


void l_pawn_shop()
{
  int i,j,k,limit,number,done = FALSE;
  char item,action;

  if (nighttime())
    print1(LS(IDS_MSG_22370));
  else {
    limit = min(5,Date-Pawndate);
    Pawndate = Date;
    for(k=0;k<limit;k++) {
      if (Pawnitems[0] != NULL) {
	if (Objects[Pawnitems[0]->id].uniqueness > UNIQUE_UNMADE)
	  Objects[Pawnitems[0]->id].uniqueness = UNIQUE_UNMADE;
	  /* could turn up anywhere, really :) */
	free((char *) Pawnitems[0]);
	Pawnitems[0] = NULL;
      }
      for (i=0;i<PAWNITEMS-1;i++)
	Pawnitems[i] = Pawnitems[i+1];
      Pawnitems[PAWNITEMS-1] = NULL;
      for (i=0;i<PAWNITEMS;i++)
 	if (Pawnitems[i] == NULL)
          do {
            if (Pawnitems[i] != NULL)
              free(Pawnitems[i]);
            do Pawnitems[i] = create_object(5);
            while (!Pawnitems[i]);
            Pawnitems[i]->known = 2;
          } while ((Pawnitems[i]->objchar == CASH) ||
                   (Pawnitems[i]->objchar == ARTIFACT) ||
                   (true_item_value(Pawnitems[i]) <= 0));
    }
    while (! done) {
      print1(LS(IDS_MSG_22371));
      print2(LS(IDS_MSG_22372));
      menuclear();
      for(i=0;i<PAWNITEMS;i++) 
	if (Pawnitems[i] != NULL) {
	  strcpy(Str3,LS(IDS_MSG_23191));
	  Str3[0] = i+'a';
	  strcat(Str3,itemid(Pawnitems[i]));
	  menuprint(Str3);
	  menuprint("\n");
	}
      showmenu();
      action = (char) mcigetc();
      if (action == ESCAPE) 
	done = TRUE;
      else if (action == 'b') {
	print2(LS(IDS_MSG_22245));
	item = ' ';
	while ((item != ESCAPE) &&
	       ((item < 'a') || (item >= 'a' + PAWNITEMS)))
	  item = (char) mcigetc();
	if (item != ESCAPE) {
	  i = item - 'a';
	  if (Pawnitems[i] == NULL) print3(LS(IDS_MSG_22373));
	  else if (true_item_value(Pawnitems[i]) <= 0) {
	    print1(LS(IDS_MSG_22374));
	    print2(LS(IDS_MSG_22375));
	    free((char *) Pawnitems[i]);
	    Pawnitems[i] = NULL;
	  }
	  else {
	    clearmsg();
	    print1(LS(IDS_MSG_22376));
	    mlongprint(Pawnitems[i]->number*true_item_value(Pawnitems[i]));
	    nprint1(LS(IDS_MSG_22377));
	    if (ynq1() == 'y') {
	      if (Player.cash < 
		  Pawnitems[i]->number *
		  true_item_value(Pawnitems[i])) {
		print2(LS(IDS_MSG_22378));
		morewait();
	      }
	      else {
		Player.cash -= 
		  Pawnitems[i]->number*
		    true_item_value(Pawnitems[i]);
		Objects[Pawnitems[i]->id].known = 1;
		gain_item(Pawnitems[i]);
		Pawnitems[i] = NULL;
	      }
	    }
	  }
	}
      }
      else if (action == 's') {
	menuclear();
	print2(LS(IDS_MSG_22379));
	i = getitem(NULL_ITEM);
	if ((i != ABORT) && (Player.possessions[i] != NULL)) {
	  if (cursed(Player.possessions[i])) {
	    print1(LS(IDS_MSG_22380));
	    morewait();
	  }
	  else if (true_item_value(Player.possessions[i]) <= 0) {
	    print1(LS(IDS_MSG_22381));
	    morewait();
	  }
	  else {
	    clearmsg();
	    print1(LS(IDS_MSG_22382));
	    mlongprint(item_value(Player.possessions[i]) / 2);
	    nprint1(LS(IDS_MSG_22383));
	    if (ynq1() == 'y') {
	      number = getnumber(Player.possessions[i]->number);
	      if ((number >= Player.possessions[i]->number) &&
		  Player.possessions[i]->used) {
		Player.possessions[i]->used = FALSE;
		item_use(Player.possessions[i]);
	      }
	      Player.cash += number * item_value(Player.possessions[i]) / 2;
	      free((char *) Pawnitems[0]);
	      for(j=0;j<PAWNITEMS - 1;j++)
		Pawnitems[j] = Pawnitems[j+1];
	      Pawnitems[PAWNITEMS - 1] = ((pob) checkmalloc(sizeof(objtype)));
	      *(Pawnitems[PAWNITEMS - 1]) = *(Player.possessions[i]);
	      Pawnitems[PAWNITEMS - 1]->number = number;
	      Pawnitems[PAWNITEMS - 1]->known = 2;	
	      dispose_lost_objects(number,Player.possessions[i]);
	      dataprint();
	    }
	  }
	}
      }
      else if (action == 'p') {
	for(i=0;i<Player.packptr;i++) {
	  if (Player.pack[i]->blessing > -1 &&
	      true_item_value(Player.pack[i]) > 0) {
	    clearmsg();
	    print1(LS(IDS_MSG_21189));
	    nprint1(itemid(Player.pack[i]));
	    nprint1(LS(IDS_MSG_21190));
	    mlongprint(item_value(Player.pack[i])/2);
	    nprint1(LS(IDS_MSG_22384));
	    if (ynq1()=='y') {
	      number = getnumber(Player.pack[i]->number);
	      if (number > 0) {
		Player.cash += number * item_value(Player.pack[i]) / 2;
		free((char *) Pawnitems[0]);
		for(j=0;j<PAWNITEMS - 1;j++)
		  Pawnitems[j] = Pawnitems[j+1];
		Pawnitems[PAWNITEMS - 1] = ((pob) checkmalloc(sizeof(objtype)));
		*(Pawnitems[PAWNITEMS - 1]) = *(Player.pack[i]);
		Pawnitems[PAWNITEMS - 1]->number = number;
		Pawnitems[PAWNITEMS - 1]->known = 2;
		Player.pack[i]->number -= number;
		if (Player.pack[i]->number < 1) {
		  free((char *)Player.pack[i]);
		  Player.pack[i] = NULL;
		}
		dataprint();
	      }
	    }
	  }
	}
	fixpack();
      }
    }
  }
  calc_melee();
  xredraw();
}
