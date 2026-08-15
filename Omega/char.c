/* omega copyright (C) by Laurence Raphael Brothers, 1987,1988,1989 */
/* char.c */
/* Player generation */

#if !defined(MSDOS) && !defined(_WIN32)
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#endif

#include "glob.h"

/* set player to begin with */
void initplayer()
{
  int i;
  int oldchar=FALSE;
  FILE *fd;
  char *lname;
#if !defined(MSDOS) && !defined(_WIN32)
  struct passwd *dastuff;
#endif

  lname = getlogin();
#if !defined(MSDOS) && !defined(_WIN32)
  if (!lname || strlen(lname) == 0)
  {
       dastuff = getpwuid(getuid());
       lname = dastuff->pw_name;
  }
#endif
  strcpy(Player.name,lname);
  if (Player.name[0] >= 'a' && Player.name[0] <= 'z')
       Player.name[0] += 'A'-'a'; /* capitalise 1st letter */
  Player.itemweight = 0;
  Player.food = 36; 
  Player.packptr = 0;
  Behavior = -1;
  Player.options = 0;
  for (i=0;i<MAXITEMS;i++)
    Player.possessions[i] = NULL;
  for (i=0;i<MAXPACK;i++)
    Player.pack[i] = NULL;
  for (i=0;i<NUMIMMUNITIES;i++) Player.immunity[i] = 0;
  for (i=0;i<NUMSTATI;i++) Player.status[i] = 0;
  for (i=0;i<NUMRANKS;i++) {
    Player.rank[i] = 0;
    Player.guildxp[i] = 0;
  }
  Player.patron = 0;
  Player.alignment = 0;
  Player.cash = 250;
  change_to_user_perms();
  if ((fd=omegarc_check())!=NULL) {
    fread((char *)&i,sizeof(int),1,fd);
    if (i != VERSION) {
#if defined(MSDOS) || defined(AMIGA) || defined(_WIN32)
      print1(LS(IDS_MSG_20312));
#else
      print1(LS(IDS_MSG_20313));
#endif
      morewait();
    }
    else {
      oldchar = TRUE;
      fread((char *)&Player,sizeof(Player),1,fd);
      fread((char *)&Searchnum,sizeof(int),1,fd);
      fread((char *)&Verbosity,sizeof(char),1,fd);
      strcpy(Player.name,lname);
      if (Player.name[0] >= 'a' && Player.name[0] <= 'z')
	   Player.name[0] += 'A'-'a'; /* capitalise 1st letter */
    }
    fclose(fd);
  }
  change_to_game_perms();
  if (! oldchar) {
    optionset(RUNSTOP);
    optionset(CONFIRM);
#ifdef COMPRESS_SAVE_FILES
    optionset(COMPRESS_OPTION);
#endif
#if defined(MSDOS) || defined(AMIGA) || defined(_WIN32)
    optionset(SHOW_COLOUR);
#endif
    initstats();
  }
  Searchnum = max(1,min(9,Searchnum));
  Player.hp = Player.maxhp = Player.maxcon;
  Player.mana = Player.maxmana = calcmana();
  Player.click = 1;
  strcpy(Player.meleestr,"CCBC");
  calc_melee();
  ScreenOffset = -1000;	/* to force a redraw */
}


FILE *omegarc_check()
{
  FILE *fd;
#if defined(MSDOS) || defined(AMIGA) || defined(_WIN32)
  if ((fd = fopen("omega.rc","rb")) != NULL) {
    print2(LS(IDS_MSG_20314));
#else
  sprintf(Str1, "%s/.omegarc", getenv("HOME"));
  if ((fd = fopen(Str1,"r")) != NULL) {
    print2(LS(IDS_MSG_20315));
#endif
    if (ynq2()!='y') {
      fclose(fd);
      fd = NULL;
    }
  }
  clearmsg();
  return(fd);
}

void initstats()
{
  char response;
  print1(LS(IDS_MSG_20316));
  do response = (char) mcigetc(); while ((response!='c')&&(response != 'p'));
  if (response == 'c') omegan_character_stats();
  else {
    user_character_stats();
    user_intro();
#if defined(MSDOS) || defined(AMIGA) || defined(_WIN32)
    print1(LS(IDS_MSG_20317));
#else
    print1(LS(IDS_MSG_20318));
#endif
    if (ynq1()=='y')
      save_omegarc();
  }
  xredraw();
}

void save_omegarc()
{
  int i=VERSION;
  FILE *fd;
  change_to_user_perms();
#if defined(MSDOS) || defined(AMIGA) || defined(_WIN32)
  fd = fopen("omega.rc","wb");
#else
  sprintf(Str1, "%s/.omegarc", getenv("HOME"));
  fd = fopen(Str1,"w");
#endif
  if (fd == NULL)
#if defined(MSDOS) || defined(AMIGA) || defined(_WIN32)
    print1(LS(IDS_MSG_20319));
#else
    print1(LS(IDS_MSG_20320));
#endif
  else {
    fwrite((char *)&i,sizeof(int),1,fd);
    print1(LS(IDS_MSG_20321));
    setoptions();
    fwrite((char *)&Player,sizeof(Player),1,fd);
    fwrite((char *)&Searchnum,sizeof(int),1,fd);
    fwrite((char *)&Verbosity,sizeof(char),1,fd);
    fclose(fd);
  }
  change_to_game_perms();
}



long calcmana()
{
  return(Player.pow * (long)(Player.level+1));
}


/*  npcbehavior digits 1234

4 : alignment (LAWFUL,CHAOTIC, or NEUTRAL)
3 : primary combat action (melee,missile,spell,thief,flight,1..5)
2 : competence at 4 (0..9, 0 = incompetent, 9 = masterful)
1 : conversation mode

status : 1 = dead, 2 = saved, 3 = retired, 4 = still playing
*/
int fixnpc(status)
int status;
{
  int npcbehavior=0;
  char response;
  if (status == 1) { /* player is dead, all undead are chaotic */
    npcbehavior+=CHAOTIC;
    npcbehavior+=10; /* melee */
    npcbehavior+=100*min(9,((int) (Player.level/3)));
    npcbehavior+=1000; /* threaten */
  }
  else if (Behavior >= 0)
    npcbehavior = Behavior;
  else {
    menuclear();
    menuprint("NPC Behavior Determination Module\n\n");
    menuprint("Your overall NPC behavior is:");
    if (Player.alignment < -10) {
      npcbehavior += CHAOTIC;
      menuprint("\n\n CHAOTIC");
    }
    else if (Player.alignment > 10) {
      npcbehavior += LAWFUL;
      menuprint("\n\n LAWFUL");
    }
    else {
      npcbehavior += NEUTRAL;
      menuprint("\n\n NEUTRAL");
    }
    menuprint("\n\n1: hand-to-hand combat");
    menuprint("\n2: missile combat");
    menuprint("\n3: spellcasting");
    menuprint("\n4: thieving");
    menuprint("\n5: escape");
    menuprint("\n\nEnter NPC response to combat: ");
    showmenu();
    response = '0';
    while ((response != '1') && 
	   (response != '2') &&
	   (response != '3') &&
	   (response != '4') &&
	   (response != '5'))
      response = menugetc();
    menuaddch(response);
    npcbehavior+=10*(response - '0');
    npcbehavior+=100*competence_check(response-'0');
    response = '0';
    menuclear();
    menuprint("1: threaten");
    menuprint("\n2: greet");
    menuprint("\n3: aid");
    menuprint("\n4: beg");
    menuprint("\n5: silence");
    menuprint("\n\nEnter NPC response to conversation: ");
    showmenu();
    while ((response != '1') && 
	   (response != '2') &&
	   (response != '3') &&
	   (response != '4') &&
	   (response != '5'))
      response = menugetc();
    menuaddch(response);
    npcbehavior+=1000*(response - '0');
    xredraw();
  }
  Behavior = npcbehavior;
  return(npcbehavior);
}


/* estimates on a 0..9 scale how good a player is at something */
int competence_check(attack)
int attack;  
{
  int ability = 0;
  switch(attack) {
  case 1: /* melee */
    ability += statmod(Player.str);
  case 2: /* missle */
    ability += statmod(Player.dex);
    ability += Player.rank[LEGION];
    ability += ((int) (Player.dmg / 10) - 1);
    break;
  case 3: /* spellcasting */
    ability += statmod(Player.iq);
    ability += statmod(Player.pow);
    ability += Player.rank[CIRCLE];
    ability += Player.rank[COLLEGE];
    ability += Player.rank[PRIEST];
    break;
  case 4: /* thieving */
    ability += statmod(Player.dex);
    ability += statmod(Player.agi);
    ability += Player.rank[THIEVES];
    break;
  case 5: /* escape */
    ability += 2 * statmod(Player.agi);
    break;
  }
  ability += ((int) (Player.level / 5));
  if (ability < 0) ability = 0;
  if (ability > 9) ability = 9;
  return(ability);
}

void user_character_stats()
{
  int num,iqpts=0,numints=0,ok,agipts=0,dexpts=0,powpts=0,conpts=0;
  print1(LS(IDS_MSG_20322));
  morewait();
  print1(LS(IDS_MSG_20323));
  num = (int) parsenum();
  if (num < 30) Player.str = Player.maxstr = 3;
  else if (num < 90) Player.str = Player.maxstr = num/10;
  else Player.str = Player.maxstr = 9+((num-120)/30);
  if (Player.str > 18) {
    print2(LS(IDS_MSG_20324));
    morewait();
    clearmsg();
    Player.str = Player.maxstr = 18;
  }
  
  print1(LS(IDS_MSG_20325));
  if (ynq1()=='y') {
    print1(LS(IDS_MSG_20326));
    num = (int) parsenum()/10;
    if (num > 18) {
      print2(LS(IDS_MSG_20324));
      morewait();
      clearmsg();
      num = 18;
    }
    iqpts+=num;
    numints++;
  }

  print1(LS(IDS_MSG_20327));
  if (ynq1()=='y') {
    do {
      print1(LS(IDS_MSG_20328));
      num = (int) parsenum();
      ok = (num < 100);
      if (! ok) {
	print2(LS(IDS_MSG_20329));
	morewait();
	clearmsg();
      }
    } while (! ok);
    iqpts += (num - 49)*9/50 + 9;
    numints++;
  }
  print1(LS(IDS_MSG_20330));
  if (ynq1()=='y') {
    do {
      print1(LS(IDS_MSG_20328));
      num = (int) parsenum();
      ok = (num < 100);
      if (! ok) {
	print2(LS(IDS_MSG_20329));
	morewait();
	clearmsg();
      }
    } while (! ok);
    iqpts += (num - 49)*9/50 + 9;
    numints++;
  }

  if (numints == 0) {
    print1(LS(IDS_MSG_20331));
    if (ynq1()=='y') {
      Player.iq = random_range(3)+3;      
      print2(LS(IDS_MSG_20332));
    }
    else {
      Player.iq = random_range(6)+8;
      print2(LS(IDS_MSG_20333));
    }
    morewait();
    clearmsg();
  }
  else Player.iq = iqpts/numints;
  Player.maxiq = Player.iq;
  agipts = 0;
  print1(LS(IDS_MSG_20334));
  if (ynq1()=='y') {
    agipts++;
    nprint1(LS(IDS_MSG_20335));
    if (ynq1()=='y') agipts+=2;
  }
  print1(LS(IDS_MSG_20336));
  if (ynq1()=='y') {
    agipts+=2;
    print2(LS(IDS_MSG_20337));
    if (ynq2()=='y') agipts+=4;
  }
  clearmsg();
  print1(LS(IDS_MSG_20338));
  if (ynq1()=='y') {
    agipts++;
    nprint1(LS(IDS_MSG_20339));
    if (ynq1()=='y') agipts++;
  }
  print1(LS(IDS_MSG_20340));
  if (ynq1()=='y')
    agipts+=3;
  print1(LS(IDS_MSG_20341));
  if (ynq1()=='y') {
    agipts+=2;
    nprint1(LS(IDS_MSG_20335));
    if (ynq1()=='y') agipts+=2;
  }
  print1(LS(IDS_MSG_20342));
  if (ynq1()=='y')
    agipts-=4;
  print1(LS(IDS_MSG_20343));
  if (ynq1()=='y')
    agipts-=4;
  print1(LS(IDS_MSG_20344));
  if (ynq1()!='y')
    agipts-=4;
  Player.agi = Player.maxagi = 9 + agipts/2;
  print1(LS(IDS_MSG_20345));
  if (ynq1()=='y') {
    dexpts+=2;
    print2(LS(IDS_MSG_20346));
    if (ynq2()=='y') dexpts+=4;
  }
  clearmsg();
  print1(LS(IDS_MSG_20347));
  if (ynq1()=='y') {
    dexpts+=2;
    print2(LS(IDS_MSG_20348));
    if (ynq2()=='y') dexpts+=4;
  }
  clearmsg();
  print1(LS(IDS_MSG_20349));
  if (ynq1()=='y') {
    dexpts+=2;
    print2(LS(IDS_MSG_20350));
  }
  morewait();
  clearmsg();
  print1(LS(IDS_MSG_20351));
  num = (int) parsenum();
  if (num > 125) {
    print2(LS(IDS_MSG_20352));
    morewait();
    clearmsg();
    num = 125;
  }
  dexpts += num/25;
  print1(LS(IDS_MSG_20353));
  if (ynq1()=='y')
    dexpts-=3;
  print1(LS(IDS_MSG_20354));
  if (ynq1()=='y')
    dexpts+=4;
  print1(LS(IDS_MSG_20355));
  if (ynq1()=='y')
    dexpts+=2;
  print1(LS(IDS_MSG_20356));
  if (ynq1()!='y')
    dexpts-=3;
  Player.dex = Player.maxdex = 6 + dexpts/2;
  print1(LS(IDS_MSG_20357));
  if (ynq1()!='y') 
    conpts+=4;
  else {
    nprint1(LS(IDS_MSG_20358));
    if (ynq1() == 'y') conpts -=4;
  }
  print1(LS(IDS_MSG_20359));
  if (ynq1()=='y') conpts -=4;
  else conpts +=4;
  print1(LS(IDS_MSG_20360));
  if (ynq1() =='y') conpts -=4;
  print1(LS(IDS_MSG_20361));
  if (ynq1() =='y') conpts -=2;
  print1(LS(IDS_MSG_20362));
  if (ynq1() =='y') conpts -=2;
  print1(LS(IDS_MSG_20363));
  if (ynq1() =='y') conpts -=3;
  print1(LS(IDS_MSG_20364));
  if (ynq1() =='y') conpts +=2;
  print1(LS(IDS_MSG_20365));
  num = (int) parsenum();
  if (num > 25) {
    print2(LS(IDS_MSG_20366));
    morewait();
    clearmsg();
    conpts += 8;
  }
  else if (num < 1) conpts -= 3;
  else if (num < 5) conpts += 2;
  else if (num < 10) conpts += 4;
  else conpts += 8;
  Player.con = Player.maxcon = 12 + conpts/3;
  print1(LS(IDS_MSG_20367));
  if (ynq1()=='y') {
    print2(LS(IDS_MSG_20368));
    morewait();
    clearmsg();
    powpts += 2;
  }
  print1(LS(IDS_MSG_20369));
  if (ynq1()=='y') {
    nprint1(LS(IDS_MSG_20370));
    morewait();
    powpts += 3;
  }
  print1(LS(IDS_MSG_20371));
  if (ynq1()=='y') {
    print2(LS(IDS_MSG_20372));
    morewait();
    clearmsg();
    powpts += 3;
  }
  print1(LS(IDS_MSG_20373));
  if (ynq1()=='y') {
    powpts += 3;
    nprint1(LS(IDS_MSG_20374));
    if (ynq1()=='y') {
      powpts+=7;
      print2(LS(IDS_MSG_20375));
      morewait();
      clearmsg();
    }
  }
  print1(LS(IDS_MSG_20376));
  if (ynq1()=='y') {
    powpts += 3;
    print2(LS(IDS_MSG_20377));
    morewait();
    clearmsg();
  }
  print1(LS(IDS_MSG_20378));
  if (ynq1()=='y') {
    powpts+= 6;
    print2(LS(IDS_MSG_20379));
    morewait();
    clearmsg();
  }
  print1(LS(IDS_MSG_20380));
  if (ynq1()=='y') {
    powpts+=2;
    print2(LS(IDS_MSG_20381));
    morewait();
    clearmsg();
  }
  print1(LS(IDS_MSG_20382));
  if (ynq1()=='y') {
    powpts+=2;
    nprint1(LS(IDS_MSG_20383));
    morewait();
  }
  Player.pow = Player.maxpow = 3 + powpts/2;
  print1(LS(IDS_MSG_20384));
  do Player.preference = (char) mcigetc();
  while ((Player.preference != 'm') && (Player.preference != 'f') &&
	(Player.preference != 'y') && (Player.preference != 'n')); /* :-) */
}



void omegan_character_stats()
{
  int share1,share2,i=0;
  print1(LS(IDS_MSG_20385));
  do {
    i++;
    sprintf(Str1, "You have only %d chance%s to reroll... ", 11 - i,
	(i == 10) ? "":"s");
    print2(Str1);
    Player.iq = Player.maxiq = 4 + random_range(5)+
      (share1 = random_range(6)) + (share2 = random_range(6));
    Player.pow = Player.maxpow = 4 + random_range(5) + share1 +share2;
    Player.dex = Player.maxdex = 4 + random_range(5)+
      (share1 = random_range(6)) + (share2 = random_range(6));
    Player.agi = Player.maxagi = 4 + random_range(5) + share1 +share2;
    Player.str = Player.maxstr = 4 + random_range(5)+
      (share1 = random_range(6)) + (share2 = random_range(6));
    Player.con = Player.maxcon = 4 + random_range(5) + share1 +share2;
    Player.cash = random_range(100)+random_range(100)+
      random_range(100)+random_range(100)+random_range(100);
    calc_melee();
    dataprint();
  } while ((i < 11) && (mgetc() == ESCAPE));
  clearmsg();
  print1(LS(IDS_MSG_20386));
  strcpy(Player.name,msgscanstring());
  if (Player.name[0] >= 'a' && Player.name[0] <= 'z')
    Player.name[0] += 'A'-'a'; /* capitalise 1st letter */
  print1(LS(IDS_MSG_20387));
  do Player.preference = (char) mcigetc();
  while ((Player.preference != 'm') && (Player.preference != 'f') &&
	(Player.preference != 'y') && (Player.preference != 'n')); /* :-) */

}

