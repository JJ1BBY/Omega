/* omega copyright (C) by Laurence Raphael Brothers, 1987,1988,1989 */
/* etc.c */
/* grab bag of random functions used in random places */

#include "glob.h"

/* there are various ways for the player to receive one of these hints */
void hint()
{
  switch(random_range(96)) {
  case 0:mprint(LS(IDS_MSG_20927));break;
  case 1:mprint(LS(IDS_MSG_20928));break;
  case 2:mprint(LS(IDS_MSG_20929));break;
  case 3:mprint(LS(IDS_MSG_20930));break;
  case 4:mprint(LS(IDS_MSG_20931));break;
  case 5:mprint(LS(IDS_MSG_20932));break;
  case 6:mprint(LS(IDS_MSG_20933));break;
  case 7:mprint(LS(IDS_MSG_20934)); break;
  case 8:mprint(LS(IDS_MSG_20935));break;
  case 9:mprint(LS(IDS_MSG_20936));break;
  case 10:mprint(LS(IDS_MSG_20937));break;
  case 11:mprint(LS(IDS_MSG_20938));break;
  case 12:mprint(LS(IDS_MSG_20939));break;
  case 13:mprint(LS(IDS_MSG_20940));break;
  case 14:mprint(LS(IDS_MSG_20941));break;
  case 15:mprint(LS(IDS_MSG_20942));break;
  case 16:mprint(LS(IDS_MSG_20943)); break;
  case 17:mprint(LS(IDS_MSG_20944));break;
  case 18:mprint(LS(IDS_MSG_20945)); break;
  case 19:mprint(LS(IDS_MSG_20946)); break;
  case 20:mprint(LS(IDS_MSG_20947));break;
  case 21:mprint(LS(IDS_MSG_20948));break;
  case 22:mprint(LS(IDS_MSG_20949)); break;
  case 23:mprint(LS(IDS_MSG_20950)); break;
  case 24:mprint(LS(IDS_MSG_20951)); break;
  case 25:mprint(LS(IDS_MSG_20952)); break;
  case 26:mprint(LS(IDS_MSG_20953)); break;
  case 27:mprint(LS(IDS_MSG_20954)); break;
  case 28:mprint(LS(IDS_MSG_20955));
    break;
  case 29:mprint(LS(IDS_MSG_20956)); break;
  case 30:mprint(LS(IDS_MSG_20957));break;
  case 31:mprint(LS(IDS_MSG_20958));break;
  case 32:mprint(LS(IDS_MSG_20959)); break;
  case 33:mprint(LS(IDS_MSG_20960)); break;
  case 34:mprint(LS(IDS_MSG_20961)); break;
  case 35:mprint(LS(IDS_MSG_20962)); break;
  case 36:mprint(LS(IDS_MSG_20963)); break;
  case 37:mprint(LS(IDS_MSG_20964)); break;
  case 38:mprint(LS(IDS_MSG_20965)); break;
  case 39:mprint(LS(IDS_MSG_20966)); break;
  case 40:mprint(LS(IDS_MSG_20967)); break;
  case 41:mprint(LS(IDS_MSG_20968)); break;
  case 42:mprint(LS(IDS_MSG_20969)); break;
  case 43:mprint(LS(IDS_MSG_20970)); break;
  case 44:mprint(LS(IDS_MSG_20971)); break;
  case 45:mprint(LS(IDS_MSG_20972)); break;
  case 46:mprint(LS(IDS_MSG_20973)); break;
  case 47:mprint(LS(IDS_MSG_20974)); break;
  case 48:mprint(LS(IDS_MSG_20975)); break;
  case 49:mprint(LS(IDS_MSG_20976)); break;
  case 50:mprint(LS(IDS_MSG_20977)); break;
  case 51:mprint(LS(IDS_MSG_20978)); break;
  case 52:mprint(LS(IDS_MSG_20979)); break;
  case 53:mprint(LS(IDS_MSG_20980)); break;
  case 54:mprint(LS(IDS_MSG_20981)); break;
  case 55:mprint(LS(IDS_MSG_20982)); break;
  case 56:mprint(LS(IDS_MSG_20983)); break;
  case 57:mprint(LS(IDS_MSG_20984)); break;
  case 58:mprint(LS(IDS_MSG_20985)); break;
  case 59:mprint(LS(IDS_MSG_20986));break;
  case 60:mprint(LS(IDS_MSG_20987));break;
  case 61:mprint(LS(IDS_MSG_20988)); break;
  case 62:mprint(LS(IDS_MSG_20989));
    break;
  case 63:mprint(LS(IDS_MSG_20990)); break;
  case 64:mprint(LS(IDS_MSG_20991)); break;
  case 65:mprint(LS(IDS_MSG_20992)); break;
  case 66:mprint(LS(IDS_MSG_20993)); break;
  case 67:mprint(LS(IDS_MSG_20994));break;
  case 68:mprint(LS(IDS_MSG_20995)); break;
  case 69:mprint(LS(IDS_MSG_20996));
    break;
  case 70:mprint(LS(IDS_MSG_20997));
    break;
  case 71:mprint(LS(IDS_MSG_20998));break;
  case 72:mprint(LS(IDS_MSG_20999));break;
  case 73:mprint(LS(IDS_MSG_21000));break;
  case 74:mprint(LS(IDS_MSG_21001));break;
  case 75:mprint(LS(IDS_MSG_21002));break;
  case 76:mprint(LS(IDS_MSG_21003));break;
  case 77:mprint(LS(IDS_MSG_21004));break;
  case 78:mprint(LS(IDS_MSG_21005));break;
  case 79:mprint(LS(IDS_MSG_21006));break;
  case 80:mprint(LS(IDS_MSG_21007));break;
  case 81:mprint(LS(IDS_MSG_21008));
    break;
  case 82:mprint(LS(IDS_MSG_21009));break;
  case 83:mprint(LS(IDS_MSG_21010));
    break;
  case 84:mprint(LS(IDS_MSG_21011));
    break;
  case 85:mprint(LS(IDS_MSG_21012));break;
  case 86:mprint(LS(IDS_MSG_21013));break;
  case 87:mprint(LS(IDS_MSG_21014));break;
  case 88:mprint(LS(IDS_MSG_21015)); break;
  case 89:mprint(LS(IDS_MSG_21016));break;
  case 90:mprint(LS(IDS_MSG_21017));break;
  case 91:mprint(LS(IDS_MSG_21018));
    break;
  case 92:mprint(LS(IDS_MSG_21019));
    break;
  case 93:mprint(LS(IDS_MSG_21020));break;
  case 94:mprint(LS(IDS_MSG_21021));break;
  case 95:mprint(LS(IDS_MSG_21022)); break;
  }
}

/* for when a deity teaches spells to a devotee */
void learnclericalspells(deity,level)
int deity,level;
{
  mprint(LS(IDS_MSG_21023));
  Player.pow+=level;
  Player.maxpow+=level;
  switch(level) {
    case LAY: 
      if (deity==ODIN) 
	Spells[S_MISSILE].known = TRUE;
      else if (deity==SET)
	Spells[S_INVISIBLE].known = TRUE;
      else if (deity==ATHENA)
	Spells[S_IDENTIFY].known = TRUE;
      else if (deity==HECATE)
	Spells[S_DRAIN].known = TRUE;
      else if (deity==DRUID) {
	Spells[S_KNOWLEDGE].known = TRUE;
	Spells[S_MON_DET].known = TRUE;
      }
      break;
    case ACOLYTE: 
      if (deity==ODIN) {
	Spells[S_LBALL].known = TRUE;
	Spells[S_TRUESIGHT].known = TRUE;
      }
      else if (deity==SET) {
	Spells[S_SUMMON].known = TRUE;
	Spells[S_FIREBOLT].known = TRUE;
      }
      else if (deity==ATHENA) {
	Spells[S_HEAL].known = TRUE;
	Spells[S_SANCTUARY].known = TRUE;
      }
      else if (deity==HECATE) {
	Spells[S_SLEEP].known = TRUE;
	Spells[S_DISPEL].known = TRUE;
      }
      else if (deity==DRUID) {
	Spells[S_HEAL].known = TRUE;
	Spells[S_CURE].known = TRUE;
      }
      else if (deity==DESTINY)
	mprint(LS(IDS_MSG_21024));
      break;
    case PRIEST: 
      Spells[S_SANCTIFY].known = TRUE;
      if (deity==ODIN) {
	Spells[S_HERO].known = TRUE;
	Spells[S_HEAL].known = TRUE;
      }
      else if (deity==SET) {
	Spells[S_INVISIBLE].known = TRUE;
	Spells[S_DISPEL].known = TRUE;
      }
      else if (deity==ATHENA) {
	Spells[S_REGENERATE].known = TRUE;
	Spells[S_ACCURACY].known = TRUE;
      }
      else if (deity==HECATE) {
	Spells[S_SHADOWFORM].known = TRUE;
	Spells[S_CURE].known = TRUE;
      }
      else if (deity==DRUID) {
	Spells[S_DISRUPT].known = TRUE;
	Spells[S_ALERT].known = TRUE;
	Spells[S_CLAIRVOYANCE].known = TRUE;
      }
      else if (deity==DESTINY)
	mprint(LS(IDS_MSG_21025));
      break;
    case SPRIEST:
      Spells[S_BLESS].known = TRUE;
      if (deity == ODIN) 
	Spells[S_ACCURACY].known = TRUE;
      else if (deity == SET)
	Spells[S_SHADOWFORM].known = TRUE;
      else if (deity == ATHENA)
	Spells[S_HERO].known = TRUE;
      else if (deity == HECATE)
	Spells[S_POLYMORPH].known = TRUE;
      else if (deity == DRUID) {
	Spells[S_POLYMORPH].known = TRUE;	
	Spells[S_LEVITATE].known = TRUE;
      }
      else if (deity == DESTINY)
	mprint(LS(IDS_MSG_21026));
      break;
    case HIGHPRIEST:
      if (deity == ODIN)
	Spells[S_RESTORE].known = TRUE;
      else if (deity == SET)
	Spells[S_HELLFIRE].known = TRUE;
      else if (deity == ATHENA)
	Spells[S_HELLFIRE].known = TRUE;
      else if (deity == HECATE)
	Spells[S_DESECRATE].known = TRUE;
      else if (deity == DRUID) {
	Spells[S_DISINTEGRATE].known = TRUE;
	Spells[S_HERO].known = TRUE;
      }
      else if (deity == DESTINY) {
	mprint(LS(IDS_MSG_21027));
	mprint(LS(IDS_MSG_21028));
      }
    }
}

/* for the use of the casino slot machine */
char *slotstr(num)
int num;
{
  switch(num) {
  case 0:
    return(LS(IDS_MSG_23054));
  case 1:
    return(LS(IDS_MSG_23055));
  case 2:
    return(LS(IDS_MSG_23056));
  case 3:
    return(LS(IDS_MSG_23057));
  case 4:
    return(LS(IDS_MSG_23058));
  case 5:
    return(LS(IDS_MSG_23059));
  case 6:
    return(LS(IDS_MSG_23060));
  case 7:
    return(LS(IDS_MSG_23061));
  case 8:
    return(LS(IDS_MSG_23062));
  case 9:
    return(LS(IDS_MSG_23063));
  }
  return "Error - you should never see this...";
}

/* random names for various uses */
char *nameprint()
{
  switch(random_range(40)) {
  case 0:strcpy(Str3,LS(IDS_MSG_23014));break;
  case 1:strcpy(Str3,LS(IDS_MSG_23015));break;
  case 2:strcpy(Str3,LS(IDS_MSG_23016));break;
  case 3:strcpy(Str3,LS(IDS_MSG_23017));break;
  case 4:strcpy(Str3,LS(IDS_MSG_23018));break;
  case 5:strcpy(Str3,LS(IDS_MSG_23019));break;
  case 6:strcpy(Str3,LS(IDS_MSG_23020));break;
  case 7:strcpy(Str3,LS(IDS_MSG_23021));break;
  case 8:strcpy(Str3,LS(IDS_MSG_23022));break;
  case 9:strcpy(Str3,LS(IDS_MSG_23023));break;
  case 10:strcpy(Str3,LS(IDS_MSG_23024));break;
  case 11:strcpy(Str3,LS(IDS_MSG_23025));break;
  case 12:strcpy(Str3,LS(IDS_MSG_23026));break;
  case 13:strcpy(Str3,LS(IDS_MSG_23027));break;
  case 14:strcpy(Str3,LS(IDS_MSG_23028));break;
  case 15:strcpy(Str3,LS(IDS_MSG_23029));break;
  case 16:strcpy(Str3,LS(IDS_MSG_23030));break;
  case 17:strcpy(Str3,LS(IDS_MSG_23031));break;
  case 18:strcpy(Str3,LS(IDS_MSG_23032));break;
  case 19:strcpy(Str3,LS(IDS_MSG_23033));break;
  case 20:strcpy(Str3,LS(IDS_MSG_23034));break;
  case 21:strcpy(Str3,LS(IDS_MSG_23035));break;
  case 22:strcpy(Str3,LS(IDS_MSG_23036));break;
  case 23:strcpy(Str3,LS(IDS_MSG_23037));break;
  case 24:strcpy(Str3,LS(IDS_MSG_23038));break;
  case 25:strcpy(Str3,LS(IDS_MSG_23039));break;
  case 26:strcpy(Str3,LS(IDS_MSG_23040));break;
  case 27:strcpy(Str3,LS(IDS_MSG_23041));break;
  case 28:strcpy(Str3,LS(IDS_MSG_23042));break;
  case 29:strcpy(Str3,LS(IDS_MSG_23043));break;
  case 30:strcpy(Str3,LS(IDS_MSG_23044));break;
  case 31:strcpy(Str3,LS(IDS_MSG_23045));break;
  case 32:strcpy(Str3,LS(IDS_MSG_23046));break;
  case 33:strcpy(Str3,LS(IDS_MSG_23047));break;
  case 34:strcpy(Str3,LS(IDS_MSG_23048));break;
  case 35:strcpy(Str3,LS(IDS_MSG_23049));break;
  case 36:strcpy(Str3,LS(IDS_MSG_23050));break;
  case 37:strcpy(Str3,LS(IDS_MSG_23051));break;
  case 38:strcpy(Str3,LS(IDS_MSG_23052));break;
  case 39:strcpy(Str3,LS(IDS_MSG_23053));break;
  }
  return(Str3);
}


/* returns english string equivalent of number */
char *wordnum(num)
int num;
{
  switch(num) {
  case 0: return(LS(IDS_MSG_23064));
  case 1: return(LS(IDS_MSG_23065));
  case 2: return(LS(IDS_MSG_23066));
  case 3: return(LS(IDS_MSG_23067));
  case 4: return(LS(IDS_MSG_23068));
  case 5: return(LS(IDS_MSG_23069));
  case 6: return(LS(IDS_MSG_23070));
  case 7: return(LS(IDS_MSG_23071));
  case 8: return(LS(IDS_MSG_23072));
  case 9: return(LS(IDS_MSG_23073));
  case 10: return(LS(IDS_MSG_23074));
  default: return(LS(IDS_MSG_23075));
  }
}
