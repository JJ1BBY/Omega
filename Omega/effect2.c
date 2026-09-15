/* omega copyright (C) by Laurence Raphael Brothers, 1987,1988,1989 */
/* effect2.c */

#include "glob.h"



void knowledge(blessing)
int blessing;
{
  if (blessing < 0)
    mprint(LS(IDS_MSG_20757));
  else {
    mprint(LS(IDS_MSG_20758));
    menuclear();
    menuprint(LS(IDS_MSG_23235));
    menulongprint(calc_points());
    menuprint(LS(IDS_MSG_23236));
    if (Player.alignment == 0)
      menuprint(LS(IDS_MSG_23237));
    else if (abs(Player.alignment) < 10)
      menuprint(LS(IDS_MSG_23238));
    else if (abs(Player.alignment) < 50)
      menuprint(LS(IDS_MSG_23239));
    else if (abs(Player.alignment) < 100) ;
    else if (abs(Player.alignment) < 200)
      menuprint(LS(IDS_MSG_23240));
    else if (abs(Player.alignment) < 400)
      menuprint(LS(IDS_MSG_23241));
    else if (abs(Player.alignment) < 800)
      menuprint(LS(IDS_MSG_23242));
    else menuprint(LS(IDS_MSG_23243));
    if (Player.alignment < 0) menuprint(LS(IDS_MSG_23244));
    else if (Player.alignment > 0) menuprint(LS(IDS_MSG_23245));
    showmenu();
    morewait();
    menuclear();
    menuprint(LS(IDS_MSG_23246));
    if (Player.status[BLINDED])
      menuprint(LS(IDS_MSG_23247));
    if (Player.status[SLOWED])
      menuprint(LS(IDS_MSG_23248));
    if (Player.status[HASTED])
      menuprint(LS(IDS_MSG_23249));
    if (Player.status[DISPLACED])
      menuprint(LS(IDS_MSG_23250));
    if (Player.status[SLEPT])
      menuprint(LS(IDS_MSG_23251));
    if (Player.status[DISEASED])
      menuprint(LS(IDS_MSG_23252));
    if (Player.status[POISONED])
      menuprint(LS(IDS_MSG_23253));
    if (Player.status[BREATHING])
      menuprint(LS(IDS_MSG_23254));
    if (Player.status[INVISIBLE])
      menuprint(LS(IDS_MSG_23255));
    if (Player.status[REGENERATING])
      menuprint(LS(IDS_MSG_23256));
    if (Player.status[VULNERABLE])
      menuprint(LS(IDS_MSG_23257));
    if (Player.status[BERSERK])
      menuprint(LS(IDS_MSG_23258));
    if (Player.status[IMMOBILE])
      menuprint(LS(IDS_MSG_23259));
    if (Player.status[ALERT])
      menuprint(LS(IDS_MSG_23260));
    if (Player.status[AFRAID])
      menuprint(LS(IDS_MSG_23261));
    if (Player.status[ACCURATE])
      menuprint(LS(IDS_MSG_23262));
    if (Player.status[HERO])
      menuprint(LS(IDS_MSG_23263));
    if (Player.status[LEVITATING])
      menuprint(LS(IDS_MSG_23264));
    showmenu();
    morewait();
    menuclear();
    menuprint(LS(IDS_MSG_23265));
    if (p_immune(NORMAL_DAMAGE))
      menuprint(LS(IDS_MSG_23266));
    if (p_immune(FLAME))
      menuprint(LS(IDS_MSG_23267));
    if (p_immune(ELECTRICITY))
      menuprint(LS(IDS_MSG_23268));
    if (p_immune(COLD))
      menuprint(LS(IDS_MSG_23269));
    if (p_immune(POISON))
      menuprint(LS(IDS_MSG_23270));
    if (p_immune(ACID))
      menuprint(LS(IDS_MSG_23271));
    if (p_immune(FEAR))
      menuprint(LS(IDS_MSG_23272));
    if (p_immune(SLEEP))
      menuprint(LS(IDS_MSG_23273));
    if (p_immune(NEGENERGY))
      menuprint(LS(IDS_MSG_23274));
    if (p_immune(THEFT))
      menuprint(LS(IDS_MSG_23275));
    if (p_immune(GAZE))
      menuprint(LS(IDS_MSG_23276));
    if (p_immune(INFECTION))
      menuprint(LS(IDS_MSG_23277));
    showmenu();
    morewait();
    menuclear();
    menuprint(LS(IDS_MSG_23278));
    switch(Player.rank[LEGION]) {
    case COMMANDANT:
      menuprint(LS(IDS_MSG_23279)); 
      break;
    case COLONEL:    
      menuprint(LS(IDS_MSG_23280));
      break;
    case FORCE_LEADER:
      menuprint(LS(IDS_MSG_23281));
      break;
    case CENTURION:
      menuprint(LS(IDS_MSG_23282));
      break;
    case LEGIONAIRE:
      menuprint(LS(IDS_MSG_23283));
      break;
    }
    if (Player.rank[LEGION] > 0) {
      menuprint(LS(IDS_MSG_23284));
      menunumprint(Player.guildxp[LEGION]);
      menuprint(LS(IDS_MSG_23285));
    }
    switch(Player.rank[ARENA]) {
    case -1:
      menuprint(LS(IDS_MSG_23286));
      break;
    case CHAMPION:
      menuprint(LS(IDS_MSG_23287));
      break;
    case GLADIATOR:    
      menuprint(LS(IDS_MSG_23288));
      break;
    case RETIARIUS:
      menuprint(LS(IDS_MSG_23289));
      break;
    case BESTIARIUS:
      menuprint(LS(IDS_MSG_23290));
      break;
    case TRAINEE:
      menuprint(LS(IDS_MSG_23291));
      break;
    }
    if (Player.rank[ARENA] > 0) {
      menuprint(LS(IDS_MSG_23292));
      menunumprint(Arena_Opponent);
      menuprint(LS(IDS_MSG_23293));
    }
    switch(Player.rank[COLLEGE]) {
    case ARCHMAGE:
      menuprint(LS(IDS_MSG_23294));
      break;
    case MAGE:
      menuprint(LS(IDS_MSG_23295));
      break;
    case PRECEPTOR:
      menuprint(LS(IDS_MSG_23296));
      break;
    case STUDENT:
      menuprint(LS(IDS_MSG_23297));
      break;
    case NOVICE:
      menuprint(LS(IDS_MSG_23298));
      break;
    }
    if (Player.rank[COLLEGE] > 0) {
      menuprint(LS(IDS_MSG_23284));
      menunumprint(Player.guildxp[COLLEGE]);
      menuprint(LS(IDS_MSG_23285));
    }
    switch(Player.rank[NOBILITY]) {
    case DUKE:
      menuprint(LS(IDS_MSG_23299));
      break;
    case LORD:
      menuprint(LS(IDS_MSG_23300));
      break;
    case KNIGHT:
      menuprint(LS(IDS_MSG_23301));
      break;
    case ESQUIRE:
      menuprint(LS(IDS_MSG_23302));
      break;
    case COMMONER:
      menuprint(LS(IDS_MSG_23303));
      break;
    default:
      menuprint(LS(IDS_MSG_23304));
      break;
    }
    if (Player.rank[NOBILITY] > 1) {
      menuprint(LS(IDS_MSG_23284));
      menunumprint(Player.rank[NOBILITY] - 1);
      menuprint(ordinal(Player.rank[NOBILITY] - 1));
      menuprint(LS(IDS_MSG_23305));
    }
    else if (Player.rank[NOBILITY] == 1) {
      menuprint(LS(IDS_MSG_23306));
    }
    switch(Player.rank[CIRCLE]) {
    case -1:
      menuprint(LS(IDS_MSG_23307));
      break;
    case PRIME:
      menuprint(LS(IDS_MSG_23308));
      break;
    case HIGHSORCEROR:
      menuprint(LS(IDS_MSG_23309));
      break;
    case SORCEROR:
      menuprint(LS(IDS_MSG_23310));
      break;
    case ENCHANTER:
      menuprint(LS(IDS_MSG_23311));
      break;
    case INITIATE:
      menuprint(LS(IDS_MSG_23312));
      break;
    }
    if (Player.rank[CIRCLE] > 0) {
      menuprint(LS(IDS_MSG_23284));
      menunumprint(Player.guildxp[CIRCLE]);
      menuprint(LS(IDS_MSG_23285));
    }
    switch(Player.rank[ORDER]) {
    case -1:
      menuprint(LS(IDS_MSG_23313));
      break;
    case JUSTICIAR:
      menuprint(LS(IDS_MSG_23314));
      break;
    case PALADIN:
      menuprint(LS(IDS_MSG_23315));
      break;
    case CHEVALIER:
      menuprint(LS(IDS_MSG_23316));
      break;
    case GUARDIAN:
      menuprint(LS(IDS_MSG_23317));
      break;
    case GALLANT:
      menuprint(LS(IDS_MSG_23318));
      break;
    }
    if (Player.rank[ORDER] > 0) {
      menuprint(LS(IDS_MSG_23284));
      menunumprint(Player.guildxp[ORDER]);
      menuprint(LS(IDS_MSG_23285));
    }
    switch(Player.rank[THIEVES]) {
    case SHADOWLORD:
      menuprint(LS(IDS_MSG_23319));
      break;
    case TMASTER:
      menuprint(LS(IDS_MSG_23320));
      break;
    case THIEF:
      menuprint(LS(IDS_MSG_23321));
      break;
    case ATHIEF:
      menuprint(LS(IDS_MSG_23322));
      break;
    case TMEMBER:
      menuprint(LS(IDS_MSG_23323));
      break;
    }
    if (Player.rank[THIEVES] > 0) {
      menuprint(LS(IDS_MSG_23284));
      menunumprint(Player.guildxp[THIEVES]);
      menuprint(LS(IDS_MSG_23285));
    }
    switch(Player.rank[PRIESTHOOD]) {
      case LAY:
        menuprint(LS(IDS_MSG_23324));
	break;
      case ACOLYTE:
        menuprint(LS(IDS_MSG_23325));
	break;
      case PRIEST:
        menuprint(LS(IDS_MSG_23326));
	break;
      case SPRIEST:
        menuprint(LS(IDS_MSG_23327));
	break;
      case HIGHPRIEST:
        menuprint(LS(IDS_MSG_23328));
	break;
      }
    switch(Player.patron) {
      case ODIN:
        menuprint(LS(IDS_MSG_23329));
	break;
      case SET:
        menuprint(LS(IDS_MSG_23330));
	break;
      case ATHENA:
        menuprint(LS(IDS_MSG_23331));
	break;
      case HECATE:
        menuprint(LS(IDS_MSG_23332));
	break;
      case DRUID:
        menuprint(LS(IDS_MSG_23333));
	break;
      case DESTINY:
        menuprint(LS(IDS_MSG_23334));
	break;
    }
    if (Player.rank[PRIESTHOOD] > 0) {
      menuprint(LS(IDS_MSG_23284));
      menunumprint(Player.guildxp[PRIESTHOOD]);
      menuprint(LS(IDS_MSG_23285));
    }
    if (Player.rank[ADEPT] > 0) 
      menuprint(LS(IDS_MSG_23335));
    showmenu();
    morewait();
    xredraw();
  }
}


/* Recreates the current level */
void flux(blessing)
int blessing;
{
  mprint(LS(IDS_MSG_20759));
  if (Current_Environment == E_CITY) {
    mprint(LS(IDS_MSG_20760));
    mprint(LS(IDS_MSG_20761));
    mprint(LS(IDS_MSG_20762));
    dispel(-1);
    mprint(LS(IDS_MSG_20763));
  }
  else if (Current_Environment != Current_Dungeon)
    mprint(LS(IDS_MSG_20764));
  else {
    mprint(LS(IDS_MSG_20765));
    erase_level();
    Level->generated = FALSE;
    mprint(LS(IDS_MSG_20766));
    change_level(Level->depth-1,Level->depth,TRUE);
  }
}

/*Turns on displacement status for the player */
void displace(blessing)
int blessing;
{
  if (blessing > -1) {
      mprint(LS(IDS_MSG_20767));
      Player.status[DISPLACED] = blessing + random_range(6);
    }
  else {
    mprint(LS(IDS_MSG_20768));
    Player.status[VULNERABLE] += random_range(6) - blessing;
  }
}

  
	
void invisible(blessing)
int blessing;
{  
  if (blessing > -1) {
    mprint(LS(IDS_MSG_20769));
    Player.status[INVISIBLE]+= 2+5*blessing;
  }
  else {
    mprint(LS(IDS_MSG_20770));
    Player.status[VULNERABLE] +=
      random_range(10)+1;
  }
}


void warp(blessing)
int blessing;
{
  int newlevel;
  if (Current_Environment != Current_Dungeon) 
    mprint(LS(IDS_MSG_20771));
  else {
    mprint(LS(IDS_MSG_20772));
    newlevel = (int) parsenum();
    if (newlevel >= MaxDungeonLevels || blessing < 0 || newlevel < 1) {
      mprint(LS(IDS_MSG_20773));
      newlevel=random_range(MaxDungeonLevels - 1) + 1;
    }
    mprint(LS(IDS_MSG_20774));
    change_level(Level->depth,newlevel,FALSE);
  }
  roomcheck();
}

void alert(blessing)
 int blessing;
{
  if (blessing > -1) {
    mprint(LS(IDS_MSG_20775));
    Player.status[ALERT]+= 4+(5*blessing);
  }
  else sleep_player(abs(blessing)+3);
}

void regenerate(blessing)
int blessing;
{
  if (blessing < 0)
    heal(blessing * 10);
  else {
    mprint(LS(IDS_MSG_20776));
    Player.status[REGENERATING] += (blessing+1)*50;
  }
}

void haste(blessing)
int blessing;
{
  if (blessing > -1) {
    if (! Player.status[HASTED]) 
      mprint(LS(IDS_MSG_20777)); 
    else mprint(LS(IDS_MSG_20778));
    if (Player.status[SLOWED]) 
      Player.status[SLOWED] = 0;
    Player.status[HASTED] += (blessing*100)+random_range(250);
  }
  else {
    mprint(LS(IDS_MSG_20779));
    if (Player.status[HASTED] > 0) mprint(LS(IDS_MSG_20780));
    else Player.status[SLOWED] += random_range(250)+250;
  }
}


void recover_stat(blessing)
int blessing;
{
  if (blessing < 0) {
    mprint(LS(IDS_MSG_20781));
    switch(random_range(6)) {
      case 0: Player.str = min(Player.str-1,Player.maxstr-1); break;
      case 1: Player.con = min(Player.con-1,Player.maxcon-1); break;
      case 2: Player.dex = min(Player.dex-1,Player.maxdex-1); break;
      case 3: Player.agi = min(Player.agi-1,Player.maxagi-1); break;
      case 4: Player.iq = min(Player.iq-1,Player.maxiq-1); break;
      case 5: Player.pow = min(Player.pow-1,Player.maxpow-1); break;
    }
  }
  else {
    mprint(LS(IDS_MSG_20782));
    Player.str = max(Player.str,Player.maxstr);
    Player.con = max(Player.con,Player.maxcon);
    Player.dex = max(Player.dex,Player.maxdex);
    Player.agi = max(Player.agi,Player.maxagi);
    Player.iq = max(Player.iq,Player.maxiq);
    Player.pow = max(Player.pow,Player.maxpow);
    }
  calc_melee();
}

void augment(blessing)
int blessing;
{
  if (blessing < 0) {
    mprint(LS(IDS_MSG_20781));
    switch(random_range(6)) {
      case 0: Player.str = min(Player.str-1,Player.maxstr-1); break;
      case 1: Player.con = min(Player.con-1,Player.maxcon-1); break;
      case 2: Player.dex = min(Player.dex-1,Player.maxdex-1); break;
      case 3: Player.agi = min(Player.agi-1,Player.maxagi-1); break;
      case 4: Player.iq = min(Player.iq-1,Player.maxiq-1); break;
      case 5: Player.pow = min(Player.pow-1,Player.maxpow-1); break;
    }
  }
  else if (blessing == 0) {
    mprint(LS(IDS_MSG_20782));
    switch(random_range(6)) {
      case 0: Player.str = max(Player.str+1,Player.maxstr+1); break;
      case 1: Player.con = max(Player.con+1,Player.maxcon+1); break;
      case 2: Player.dex = max(Player.dex+1,Player.maxdex+1); break;
      case 3: Player.agi = max(Player.agi+1,Player.maxagi+1); break;
      case 4: Player.iq = max(Player.iq+1,Player.maxiq+1); break;
      case 5: Player.pow = max(Player.pow+1,Player.maxpow+1); break;
    }
  }
  else {
    mprint(LS(IDS_MSG_20783));
    Player.str = max(Player.str+1,Player.maxstr+1);
    Player.con = max(Player.con+1,Player.maxcon+1);
    Player.dex = max(Player.dex+1,Player.maxdex+1);
    Player.agi = max(Player.agi+1,Player.maxagi+1);
    Player.iq = max(Player.iq+1,Player.maxiq+1);
    Player.pow = max(Player.pow+1,Player.maxpow+1);
  }
  calc_melee();
}

void breathe(blessing)
int blessing;
{
  if (blessing > -1) {
      mprint(LS(IDS_MSG_20784));
      Player.status[BREATHING] += 6+blessing;
    }
  else {
    mprint(LS(IDS_MSG_20785));
    p_damage(50,UNSTOPPABLE,LS(IDS_MSG_23541));
  }
}


void i_chaos(o)
pob o;
{
  if (Player.alignment < 0) {
    Player.alignment -= random_range(20);
    mprint(LS(IDS_MSG_20786));
    gain_experience(abs(Player.alignment)*10);
  }
  else {
    mprint(LS(IDS_MSG_20787));
    Player.alignment -= random_range(20);
  }
}

void i_law(o)
pob o;
{
  if (Player.alignment > 0) {
    Player.alignment += random_range(20);
    mprint(LS(IDS_MSG_20788));
    gain_experience(Player.alignment*10);
  }
  else {
    mprint(LS(IDS_MSG_20789));
    Player.alignment += random_range(20);
  }
}

void sanctify(blessing)
int blessing;
{
  if (blessing > -1) {
    if (Level->environment == E_TEMPLE) 
      mprint(LS(IDS_MSG_20790));
    else if (Level->site[Player.x][Player.y].locchar == ALTAR) 
      mprint(LS(IDS_MSG_20791));
    else if (Player.patron == 0) {
      mprint(LS(IDS_MSG_20792));
      Level->site[Player.x][Player.y].locchar = LAVA;
      Level->site[Player.x][Player.y].p_locf = L_LAVA;
      lset(Player.x, Player.y, CHANGED);
      p_movefunction(L_LAVA);
    }
    else {
      Level->site[Player.x][Player.y].locchar = ALTAR;
      Level->site[Player.x][Player.y].aux = Player.patron;
      Level->site[Player.x][Player.y].p_locf = L_ALTAR;
      lset(Player.x, Player.y, CHANGED);
      mprint(LS(IDS_MSG_20793));
    }
  }
  else {
    if (Level->site[Player.x][Player.y].locchar == ALTAR) {
      mprint(LS(IDS_MSG_20794));
      Level->site[Player.x][Player.y].locchar = FLOOR;
      Level->site[Player.x][Player.y].p_locf = L_NO_OP;
      lset(Player.x, Player.y, CHANGED);
      if (Level->site[Player.x][Player.y].aux == Player.patron) {
	mprint(LS(IDS_MSG_20795));
	p_damage(Player.hp-1,UNSTOPPABLE,LS(IDS_MSG_23542));
      }
      else if ((Player.patron == ATHENA) || (Player.patron == ODIN)) {
	if ((Level->site[Player.x][Player.y].aux == SET) ||
	    (Level->site[Player.x][Player.y].aux == HECATE)) {
	  mprint(LS(IDS_MSG_20796));
	  gain_experience(1000);
	}
	else {
	  mprint(LS(IDS_MSG_20797));
	  gain_experience(100);
	}
      }
      else if ((Player.patron == SET) || (Player.patron == HECATE)) {
	if ((Level->site[Player.x][Player.y].aux == ODIN) ||
	    (Level->site[Player.x][Player.y].aux == ATHENA)) {
	  mprint(LS(IDS_MSG_20798));
	  gain_experience(1000);
	}
	else {
	  mprint(LS(IDS_MSG_20797));
	  gain_experience(100);
	}
      }
      else if (Player.patron == DRUID) {
	mprint(LS(IDS_MSG_20799));
	gain_experience(250);
      }
      else mprint(LS(IDS_MSG_20800));
    }
    else mprint(LS(IDS_MSG_20801));
  }
}

void accuracy(blessing)
int blessing;
{
  if (blessing > -1) {
      mprint(LS(IDS_MSG_20802));
      Player.status[ACCURACY] += random_range(5)+1+blessing*5;
      calc_melee();
    }
  else {
    Player.status[ACCURACY]=0;
    calc_melee();
    mprint(LS(IDS_MSG_20803));
  }
}
