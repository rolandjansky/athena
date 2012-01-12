/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigTauSelectionManager.cxx
/// Package : TrigTauPerformAnalysis
/// Author  : Eric Torrence (University of Oregon)
/// Created : February 2007
///
/// DESCRIPTION:
///           Class to manage the individual L1/L2/EF selectors.
///           Can also be used to define complete trigger menus
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "TrigTauPerformAnalysis/TrigTauSelectionManager.h"

#include "TrigTauPerformNtuple/TrigTauL1Object.h"
#include "TrigTauPerformNtuple/TrigTauL2Object.h"
#include "TrigTauPerformNtuple/TrigTauRecEFObject.h"

#include "TrigTauPerformAnalysis/TrigTauTrigChain.h"
#include "TrigTauPerformAnalysis/TrigTauL1Sel.h"
#include "TrigTauPerformAnalysis/TrigTauL2CalSel.h"
#include "TrigTauPerformAnalysis/TrigTauL2TrkSel.h"
#include "TrigTauPerformAnalysis/TrigTauEFSel.h"

#include <math.h>
#include <iostream>
using namespace std;

//
// -----------------------------------------------------------------------
//

TrigTauSelectionManager::TrigTauSelectionManager() : 
        _menuVersion( MENU1560),
        _cutLevel( LOOSE),
        _usedType( TrigTauFlags::TAU12),
        _givenType( TrigTauFlags::TAU12),
        l2CalOnly(false),
        l2TrkOnly(false) {
}

TrigTauSelectionManager::~TrigTauSelectionManager() {
}


TrigTauFlags::TRIGGERMENU TrigTauSelectionManager::usedTauMenu() const
{
  
  switch( _cutLevel ){
  case LOOSE :
    switch (_usedType )
	{
	case TrigTauFlags::TAU12   : return TrigTauFlags::TAU12   ;
	case TrigTauFlags::TAU16   : return TrigTauFlags::TAU16   ;
	case TrigTauFlags::TAU20   : return TrigTauFlags::TAU20   ;
	case TrigTauFlags::TAU29   : return TrigTauFlags::TAU29   ;
	case TrigTauFlags::TAU29v2 : return TrigTauFlags::TAU29v2 ;
	case TrigTauFlags::TAU38   : return TrigTauFlags::TAU38   ;
	case TrigTauFlags::TAU50   : return TrigTauFlags::TAU50   ;
	case TrigTauFlags::TAU84   : return TrigTauFlags::TAU84   ;
	case TrigTauFlags::TAU100  : return TrigTauFlags::TAU100  ;
	case TrigTauFlags::TAU125  : return TrigTauFlags::TAU125  ;
	default : return _usedType;
	}
    break;
  case MEDIUM :
    switch (_usedType )
	{
	case TrigTauFlags::TAU16   : return TrigTauFlags::TAU16_MEDIUM ;
	case TrigTauFlags::TAU20   : return TrigTauFlags::TAU20_MEDIUM ;
	case TrigTauFlags::TAU29   : return TrigTauFlags::TAU29_MEDIUM ;
	case TrigTauFlags::TAU38   : return TrigTauFlags::TAU38_MEDIUM ;
	case TrigTauFlags::TAU50   : return TrigTauFlags::TAU50_MEDIUM ;
	case TrigTauFlags::TAU84   : return TrigTauFlags::TAU84_MEDIUM ;
	case TrigTauFlags::TAU100  : return TrigTauFlags::TAU100_MEDIUM;
	case TrigTauFlags::TAU125  : return TrigTauFlags::TAU125_MEDIUM;
	default : return _usedType;
	}
    break;
  case TIGHT :
    switch (_usedType )
	{
	case TrigTauFlags::TAU16   : return TrigTauFlags::TAU16_TIGHT ;
	case TrigTauFlags::TAU20   : return TrigTauFlags::TAU20_TIGHT ;
	case TrigTauFlags::TAU29   : return TrigTauFlags::TAU29_TIGHT ;
	case TrigTauFlags::TAU38   : return TrigTauFlags::TAU38_TIGHT ;
	case TrigTauFlags::TAU50   : return TrigTauFlags::TAU50_TIGHT ;
	case TrigTauFlags::TAU84   : return TrigTauFlags::TAU84_TIGHT ;
	case TrigTauFlags::TAU125  : return TrigTauFlags::TAU125_TIGHT;
	default : return _usedType;
	}
    break;
  case MEDIUM1 :
    switch (_usedType )
	{
	case TrigTauFlags::TAU16   : return TrigTauFlags::TAU16_MEDIUM1 ;
	case TrigTauFlags::TAU20   : return TrigTauFlags::TAU20_MEDIUM1 ;
	case TrigTauFlags::TAU29   : return TrigTauFlags::TAU29_MEDIUM1 ;
	case TrigTauFlags::TAU100  : return TrigTauFlags::TAU100_MEDIUM1 ;
	default : return _usedType;
	}
  default : return _usedType;
  }
}


void
TrigTauSelectionManager::setTauMenu(TrigTauFlags::TRIGGERMENU givenType, CUTLEVEL level) {

  // ==================================================================================
  // First of all, let us decide on which CUTLEVEl to use
  // ==================================================================================
  TrigTauFlags::TRIGGERMENU type = givenType;

  if( level == DEFAULT )
    {
      level=LOOSE;
      // guess level from the name
      switch (givenType )
	{
	case TrigTauFlags::TAUNOCUT :level = SOFT; break;
	case TrigTauFlags::TAUNOCUT_CALO :level = SOFT; break;
	case TrigTauFlags::TRK9    : level = LOOSE; break;
	case TrigTauFlags::TAU12   : level = LOOSE; break;
	case TrigTauFlags::TAU16   : level = LOOSE; break;
	case TrigTauFlags::TAU20   : level = LOOSE; break;
	case TrigTauFlags::TAU29   : level = LOOSE; break;
	case TrigTauFlags::TAU29v2 : level = LOOSE; break;
	case TrigTauFlags::TAU38   : level = LOOSE; break;
	case TrigTauFlags::TAU50   : level = LOOSE; break;
	case TrigTauFlags::TAU84   : level = LOOSE; break;
	case TrigTauFlags::TAU100  : level = LOOSE; break;
	case TrigTauFlags::TAU125  : level = LOOSE; break;

	case TrigTauFlags::TAU16_MEDIUM:
          level = MEDIUM; type=TrigTauFlags::TAU16 ; break;
	case TrigTauFlags::TAU20_MEDIUM:
          level = MEDIUM; type=TrigTauFlags::TAU20 ; break;
	case TrigTauFlags::TAU29_MEDIUM:
          level = MEDIUM; type=TrigTauFlags::TAU29 ; break;
	case TrigTauFlags::TAU38_MEDIUM:
          level = MEDIUM; type=TrigTauFlags::TAU38 ; break;
	case TrigTauFlags::TAU50_MEDIUM:
          level = MEDIUM; type=TrigTauFlags::TAU50 ; break;
	case TrigTauFlags::TAU84_MEDIUM:
          level = MEDIUM; type=TrigTauFlags::TAU84 ; break;
	case TrigTauFlags::TAU100_MEDIUM:
          level = MEDIUM; type=TrigTauFlags::TAU100; break;
	case TrigTauFlags::TAU125_MEDIUM:
          level = MEDIUM; type=TrigTauFlags::TAU125; break;

	case TrigTauFlags::TAU16_TIGHT:
          level = TIGHT; type=TrigTauFlags::TAU16 ; break;
	case TrigTauFlags::TAU20_TIGHT:
          level = TIGHT; type=TrigTauFlags::TAU20 ; break;
	case TrigTauFlags::TAU29_TIGHT:
          level = TIGHT; type=TrigTauFlags::TAU29 ; break;
	case TrigTauFlags::TAU38_TIGHT:
          level = TIGHT; type=TrigTauFlags::TAU38 ; break;
	case TrigTauFlags::TAU50_TIGHT:
          level = TIGHT; type=TrigTauFlags::TAU50 ; break;
	case TrigTauFlags::TAU84_TIGHT:
          level = TIGHT; type=TrigTauFlags::TAU84 ; break;
	case TrigTauFlags::TAU125_TIGHT:
          level = TIGHT; type=TrigTauFlags::TAU125; break;

	case TrigTauFlags::TAU16_MEDIUM1:
          level = MEDIUM1; type=TrigTauFlags::TAU16 ; break;
	case TrigTauFlags::TAU20_MEDIUM1:
          level = MEDIUM1; type=TrigTauFlags::TAU20 ; break;
	case TrigTauFlags::TAU29_MEDIUM1:
          level = MEDIUM1; type=TrigTauFlags::TAU29 ; break;
	case TrigTauFlags::TAU100_MEDIUM1:
          level = MEDIUM1; type=TrigTauFlags::TAU100 ; break;

	default:
          cout << "ERROR: please specify single tau menu for"
               << " TrigTauSelectionManager::setTauMenu" << std::endl;
          break;
	}



    }else{

    static bool first = true;
      switch (givenType )
	{
	case TrigTauFlags::TAUNOCUT: 
	case TrigTauFlags::TRK9    : 
	case TrigTauFlags::TAU12   : 
	case TrigTauFlags::TAU16   : 
	case TrigTauFlags::TAU20   : 
	case TrigTauFlags::TAU29   : 
	case TrigTauFlags::TAU29v2 : 
	case TrigTauFlags::TAU38   : 
	case TrigTauFlags::TAU50   : 
	case TrigTauFlags::TAU84   : 
	case TrigTauFlags::TAU100  : 
	case TrigTauFlags::TAU125  : 
	  if( level != LOOSE ) 
	    { if (first){cout << "TrigTauSelectionManager ERROR : menu " 
		  << TrigTauFlags::menuName(givenType) 
			      << " should be used with LOOSE ("
			      << LOOSE<<") or DEFAULT level ("<< LOOSE <<") \n"
		  << " using " << level << "instead \n"; 	
		first = false;
	      }
	      break;
	    }
	  break;  

	case TrigTauFlags::TAU16_MEDIUM : 
	case TrigTauFlags::TAU20_MEDIUM : 
	case TrigTauFlags::TAU29_MEDIUM : 
	case TrigTauFlags::TAU38_MEDIUM : 
	case TrigTauFlags::TAU50_MEDIUM : 
	case TrigTauFlags::TAU84_MEDIUM : 
	case TrigTauFlags::TAU100_MEDIUM: 
	case TrigTauFlags::TAU125_MEDIUM: 
	  if( level != MEDIUM ) 
	    { if (first){ cout << "TrigTauSelectionManager ERROR : menu " 
				    << TrigTauFlags::menuName(givenType) 
				    << " should be used with MEDIUM ("
			      << MEDIUM<<") or DEFAULT level ("<< LOOSE <<") \n"
				     << " using " << level << "instead \n"; 	
		first = false;
	      }
	      break;
	    }
	  break;
	case TrigTauFlags::TAU16_TIGHT : 
	case TrigTauFlags::TAU20_TIGHT : 
	case TrigTauFlags::TAU29_TIGHT : 
	case TrigTauFlags::TAU38_TIGHT : 
	case TrigTauFlags::TAU50_TIGHT : 
	case TrigTauFlags::TAU84_TIGHT : 
	case TrigTauFlags::TAU125_TIGHT: 
	  if( level != TIGHT ) 
	    { if (first){  cout << "TrigTauSelectionManager ERROR : menu " 
				    << TrigTauFlags::menuName(givenType) 
				    << " should be used with TIGHT ("
			      << TIGHT<<") or DEFAULT level ("<< LOOSE <<") \n"
				    << " using " << level << "instead \n";	
		first = false;
	      }
	       break;
	    }
	  break;
	case TrigTauFlags::TAU16_MEDIUM1: 
	case TrigTauFlags::TAU20_MEDIUM1: 
	case TrigTauFlags::TAU29_MEDIUM1: 
	case TrigTauFlags::TAU100_MEDIUM1: 
	  if( level != MEDIUM1 ) 
	    { if (first){ cout << "TrigTauSelectionManager ERROR : menu " 
				    << TrigTauFlags::menuName(givenType) 
				    << " should be used with MEDIUM1 ("
			      << MEDIUM1<<") or DEFAULT level ("<< LOOSE <<") \n"
				     << " using " << level << "instead \n"; 	
		first = false;
	      }
	      break;
	    }
	  break;

	default : cout << "ERROR: please specify single tau menu for TrigTauSelectionManager::setTauMenu\n"; break;
	}

      if( givenType == TrigTauFlags::TAU16_MEDIUM1 ||
          givenType == TrigTauFlags::TAU16_MEDIUM  ||
          givenType ==  TrigTauFlags::TAU16_TIGHT)
          type = TrigTauFlags::TAU16 ; 
      if( givenType == TrigTauFlags::TAU20_MEDIUM1 ||
          givenType == TrigTauFlags::TAU20_MEDIUM  ||
          givenType == TrigTauFlags::TAU20_TIGHT)
          type = TrigTauFlags::TAU20 ; 
      if( givenType == TrigTauFlags::TAU29_MEDIUM1 ||
          givenType == TrigTauFlags::TAU29_MEDIUM  ||
          givenType == TrigTauFlags::TAU29_TIGHT)
          type = TrigTauFlags::TAU29 ; 
      if( givenType == TrigTauFlags::TAU38_MEDIUM ||
          givenType == TrigTauFlags::TAU38_TIGHT)
          type = TrigTauFlags::TAU38 ; 
      if( givenType == TrigTauFlags::TAU50_MEDIUM ||
          givenType == TrigTauFlags::TAU50_TIGHT)
          type = TrigTauFlags::TAU50 ; 
      if( givenType == TrigTauFlags::TAU84_MEDIUM ||
          givenType == TrigTauFlags::TAU84_TIGHT)
          type = TrigTauFlags::TAU84 ; 
      if( givenType == TrigTauFlags::TAU100_MEDIUM ||
          givenType == TrigTauFlags::TAU100_MEDIUM1)
          type = TrigTauFlags::TAU100 ; 
      if( givenType == TrigTauFlags::TAU125_MEDIUM ||
          givenType == TrigTauFlags::TAU125_TIGHT)
          type = TrigTauFlags::TAU125 ; 
  }

  _cutLevel  = level;
  _givenType = givenType;
  _usedType  = type;


  switch (_menuVersion ) {


  case MENU1206 :     setTauMenu1206 (_usedType, level ); break ; 
  case MENUJUNE06:
  case MENUJUNE07:    setTauMenuJUNE07 (_usedType, level ); break ; 
  case MENU13020:     setTauMenu13020 (_usedType, level ); break ;
  case MENUAUG07:     setTauMenuAUG07 (_usedType, level ) ;break ; 
  case MENU13030pre:  setTauMenu13030pre (_usedType, level ); break ; 
  case MENU1400:      setTauMenu1400 (_usedType, level ); break ; 
  case MENU1560:      setTauMenu1560 (_usedType, level ); break ; 
  case MENU1520test:  setTauMenu1520test (_usedType, level ); break ; 
  case MENU1600:      setTauMenu1600 (_usedType, level ); break ; 
    //TrigTauHypo-00-02-34
  case MENU1662:      setTauMenu1662 (_usedType, level ); break ;
    //TrigTauHypo-00-02-42, BUT TEMPORARILY TAU125_MEDIUM was modified and  TAU100_MEDIUM1 was added
  case MENU1666:      setTauMenu1666 (_usedType, level ); break ;
    //TrigTauHypo-00-03-02
  case MENUTEST:      setTauMenuTest (_usedType, level ); break ;

  default : 
    cout << "Unknown menu version " << _menuVersion 
	 << ", menus not set!" << endl;
    break;
  }

  
}

//=============================================================================================
void TrigTauSelectionManager::useHLTTriggerBits(bool useBits) {

  if (useBits) {
    l2CalSel.useBits(_givenType);
    l2TrkSel.useBits(_givenType);
    efSel.useBits(_givenType);
  } else {
    // Clear the selectors from using trigger bits
    l2CalSel.useBits(TrigTauFlags::SECONDATTEMPT);
    l2TrkSel.useBits(TrigTauFlags::SECONDATTEMPT);
    efSel.useBits(TrigTauFlags::SECONDATTEMPT);
  }

}

//=============================================================================================
void TrigTauSelectionManager::setTauMenu1206(TrigTauFlags::TRIGGERMENU type, CUTLEVEL /*level*/) {
  switch (type) {
  case TrigTauFlags::TAUNOCUT: // Not really "no" cuts
    l1Sel.setCuts(0, 5000, 0, 0, 0, 0);
    l2CalSel.setCuts(0, 0, 0, 0.0, 0.0, 0.0, 0);
    l2TrkSel.setCuts(-1, -1, -1, -1, 7500, 0.1, 1500, 0.15, 0.30);
    efSel.setCuts(0, 100, 100., 10., -10., 0.);
    break;

  case TrigTauFlags::TAU12:
      l1Sel.setCuts(0, 13000, 0, 0, 0, 0);
      l2CalSel.setCuts(0, 11000, 0, 0.055, 0.0127, 0.425, 0);
      l2TrkSel.setCuts(4, 0, 2, 3, 7500, 0.15, 1500, 0.15, 0.30);
      efSel.setCuts(1, 3, 0.15, 0.4, 0., 5000.);
    break;

//   case TrigTauFlags::TAU16I:
//       l1Sel.setCuts(0, 17000, 6000, 0, 0, 0);
//       l2CalSel.setCuts(0, 17000, 0, 0.055, 0.0127, 0.425, 0);
//       l2TrkSel.setCuts(4, 0, 2, 3, 7500, 0.15, 1500, 0.15, 0.30);
//       efSel.setCuts(1, 3, 0.08, 0.31, 0., 5000.);
//     break;
    
//   case TrigTauFlags::TAU20I:
//       l1Sel.setCuts(0, 22000, 6000, 0, 0, 0);
//       l2CalSel.setCuts(0, 20000, 0, 0.055, 0.0127, 0.425, 0);
//       l2TrkSel.setCuts(4, 0, 2, 3, 7500, 0.15, 1500, 0.15, 0.30);
//       efSel.setCuts(1, 3, 0.08, 0.31, 0., 5000.);
//     break;

//   case TrigTauFlags::TAU29I:
//       l1Sel.setCuts(0, 30000, 6000, 0, 0, 0);
//       l2CalSel.setCuts(0, 23300, 0, 0.055, 0.0127, 0.425, 0);
//       l2TrkSel.setCuts(4, 0, 1, 3, 7500, 0.15, 1500, 0.15, 0.30);
//       efSel.setCuts(1, 3, 0.08, 0.31, 0., 5000.);
//     break;

  case TrigTauFlags::TAU38:
//   case TrigTauFlags::TAU38I:     
//       if( type ==  TrigTauFlags::TAU38I)   l1Sel.setCuts(0, 21000, 0, 0, 0, 0);
//       else                                
      l1Sel.setCuts(0, 21000, 0, 0, 0, 0);
      // l2CalSel.setCuts(0, 33500, 0, 0.060, 0.0100, 0.60, 0);
      l2CalSel.setCuts(0, 28500, 0, 0.060, 0.0100, 0.60, 0);
      l2TrkSel.setCuts(7, -1, 1, 7, 7500, 0.15, 1500, 0.15, 0.30, 5000, true, 0.2);
      l2TrkSel.setCoreFromLeading(true);
      efSel.setCuts(1, 8, 0.14, 0.4, 0., 7000., 35000.);
    break;

  default :
      cout << ":setTauMenu can not deal with menu " <<  TrigTauFlags::menuName(type) << endl;

  }
}

//=============================================================================================
void TrigTauSelectionManager::setTauMenuJUNE07(TrigTauFlags::TRIGGERMENU type, CUTLEVEL /*level*/) {
  switch (type) {
  case TrigTauFlags::TAUNOCUT: // Not really "no" cuts
    l1Sel.setCuts(0, 5000, 0, 0, 0, 0);
    l2CalSel.setCuts(0, 0, 0, 0.0, 0.0, 0.0, 0);
    l2TrkSel.setCuts(-1, -1, -1, -1, 7500, 0.1, 1500, 0.15, 0.30);
    efSel.setCuts(0, 100, 100., 10., -10., 0.);
    break;


  case TrigTauFlags::TAU12:
      l1Sel.setCuts(0, 7000, 0, 0, 0, 0);
      l2CalSel.setCuts(0, 11000, 0, 0.2, 0.1, 1., 0);
      l2TrkSel.setCuts(4, 0, 2, 3, 7500, 0.15, 1500, 0.15, 0.30, 2500.);
      efSel.setCuts(1, 3, 0.15, 0.4, 0., 3000., 15000.);
    break;

//   case TrigTauFlags::TAU16I:
//       l1Sel.setCuts(0, 9000, 6000, 0, 0, 0);
//       l2CalSel.setCuts(0, 14000, 0, 0.0670, 0.0218, 0.61500, 0);
//       l2TrkSel.setCuts(4, 0, 2, 3, 7500, 0.15, 1500, 0.15, 0.30, 5000.);
//       efSel.setCuts(1, 3, 0.08, 0.31, 0., 5000., 20000.);
//     break;
    
//   case TrigTauFlags::TAU20I:
//       l1Sel.setCuts(0, 11000, 6000, 0, 0, 0);
//       l2CalSel.setCuts(0, 18000, 0, 0.061, 0.01945, 0.6150, 0);
//       l2TrkSel.setCuts(4, 0, 1, 3, 7500, 0.15, 1500, 0.15, 0.30, 5000.);
//       efSel.setCuts(1, 3, 0.08, 0.31, 0., 5000., 25000.);
//     break;

//   case TrigTauFlags::TAU29I:
//       l1Sel.setCuts(0, 16000, 6000, 0, 0, 0);
//       l2CalSel.setCuts(0, 25000, 0, 0.052, 0.0124, 0.545, 0);
//       l2TrkSel.setCuts(4, 0, 1, 3, 7500, 0.15, 1500, 0.15, 0.30, 5000.);
//       efSel.setCuts(1, 3, 0.08, 0.31, 0., 5000., 35000.);
//     break;

  case TrigTauFlags::TAU38:
//   case TrigTauFlags::TAU38I:     
//       if( type ==  TrigTauFlags::TAU38I)   l1Sel.setCuts(0, 21000, 0, 0, 0, 0);
//       else                                 
      l1Sel.setCuts(0, 21000, 0, 0, 0, 0);
      l2CalSel.setCuts(0, 0, 0, 0.060, 0.0100, 0.60, 0);
      l2TrkSel.setCuts(7, -1, 1, 7, 7500, 0.15, 1500, 0.15, 0.30, 5000, true, 0.2, 45000.);
      l2TrkSel.setCoreFromLeading(true);
      efSel.setCuts(1, 8, 0.14, 0.4, 0., 7000., 35000.);
    break;

  default :
      cout << ":setTauMenu can not deal with menu " << TrigTauFlags::menuName(type)  << endl;

  }

}

//=============================================================================================
void TrigTauSelectionManager::setTauMenu13020(TrigTauFlags::TRIGGERMENU type, CUTLEVEL /*level*/) {
  switch (type) {
  case TrigTauFlags::TAUNOCUT: // Not really "no" cuts
    l1Sel.setCuts(0, 5000, 0, 0, 0, 0);
    l2CalSel.setCuts(0, 0, 0, 0.0, 0.0, 0.0, 0);
    l2TrkSel.setCuts(-1, -1, -1, -1, 7500, 0.1, 1500, 0.15, 0.30);
    efSel.setCuts(0, 100, 100., 10., -10., 0.);
    break;

  case TrigTauFlags::TAU12:
      l1Sel.setCuts(0, 6000, 0, 0, 0, 0);
      l2CalSel.setCuts(0, 9700, 0, 0.065, 0.0160, 0.71, 0);
      l2TrkSel.setCuts(7, -1, 2, 7, 7500, 0.15, 1500, 0.15, 0.30, 2500, true, 0.1);
      l2TrkSel.setCoreFromLeading(true);
      efSel.setCuts(1, 8, 0.20, 0.4, 0., 5000., 15000.);
    break;

//   case TrigTauFlags::TAU16I:
//       l1Sel.setCuts(0, 9000, 6000, 0, 0, 0);
//       l2CalSel.setCuts(0, 12200, 0, 0.055, 0.0150, 0.6000, 0);
//       l2TrkSel.setCuts(7, -1, 2, 7, 7500, 0.15, 1500, 0.15, 0.30, 5000, true, 0.1);
//       l2TrkSel.setCoreFromLeading(true);
//       efSel.setCuts(1, 8, 0.11, 0.29, 0., 5000., 19000.);
//    break;
    
//   case TrigTauFlags::TAU20I:
//       l1Sel.setCuts(0, 11000, 6000, 0, 0, 0);
//       l2CalSel.setCuts(0, 17000, 0, 0.053, 0.0140, 0.56, 0);
//       //      l2CalSel.setCuts(0, 12000, 0, 0.053, 0.0140, 0.56, 0);
//       l2TrkSel.setCuts(7, -1, 1, 7, 7500, 0.15, 1500, 0.15, 0.30, 5000, true, 0.1);
//       l2TrkSel.setCoreFromLeading(true);
//       efSel.setCuts(1, 8, 0.1, 0.27, 0., 7000., 19000.);
//     break;

//   case TrigTauFlags::TAU29I:
//       l1Sel.setCuts(0, 16000, 6000, 0, 0, 0);
//       l2CalSel.setCuts(0, 26500, 0, 0.053, 0.0095, 0.50, 0);
//       // l2CalSel.setCuts(0, 21500, 0, 0.053, 0.0095, 0.50, 0);
//       l2TrkSel.setCuts(7, -1, 1, 7, 7500, 0.15, 1500, 0.15, 0.30, 5000, true, 0.1);
//       l2TrkSel.setCoreFromLeading(true);
//       efSel.setCuts(1, 8, 0.10, 0.22, 0., 7000., 27000.);
//     break;

  case TrigTauFlags::TAU38:
      l1Sel.setCuts(0, 25000, 0, 0, 0, 0);
      l2CalSel.setCuts(0, 33500, 0, 0.06, 0.01, 0.6, 0);
      l2TrkSel.setCuts(7, -1, 2, 7, 7500, 0.15, 1500, 0.15, 0.30, 5000, true, 0.2, 0., 1);
      l2TrkSel.setCoreFromLeading(true);
      efSel.setCuts(1, 8, 0.12, 0.3, 0., 8000., 36000.);
    break;

//   case TrigTauFlags::TAU38I:
//       l1Sel.setCuts(0, 21000, 6000, 0, 0, 0);
//       l2CalSel.setCuts(0, 33500, 0, 0.048, 0.0087, 0.43, 0);
//       // l2CalSel.setCuts(0, 28500, 0, 0.048, 0.0087, 0.43, 0);
//       l2TrkSel.setCuts(7, -1, 1, 7, 7500, 0.15, 1500, 0.15, 0.30, 5000, true, 0.1);
//       l2TrkSel.setCoreFromLeading(true);
//       efSel.setCuts(1, 8, 0.10, 0.22, 0., 7000., 35000.);
//     break;  

  case TrigTauFlags::TAU50:
      l1Sel.setCuts(0, 35000, 0, 0, 0, 0);
      l2CalSel.setCuts(0, 44900, 0, 0.043, 0.0046, 0.35, 0);
      // l2CalSel.setCuts(0, 40000, 0, 0.043, 0.0046, 0.35, 0);
      l2TrkSel.setCuts(7, -1, 1, 7, 7500, 0.15, 1500, 0.15, 0.30, 5000, true, 0.2);
      l2TrkSel.setCoreFromLeading(true);
      efSel.setCuts(1, 8, 0.14, 0.4, 0., 7000., 45000.);
    break;

  default :
      cout << ":setTauMenu can not deal with menu " <<  TrigTauFlags::menuName(type)  << endl;

  }

} 



//=============================================================================================
void TrigTauSelectionManager::setTauMenuAUG07(TrigTauFlags::TRIGGERMENU type, CUTLEVEL /*level*/) {
  switch (type) {
  case TrigTauFlags::TAUNOCUT: // Not really "no" cuts
    l1Sel.setCuts(0, 5000, 0, 0, 0, 0);
    l2CalSel.setCuts(0, 0, 0, 0.0, 0.0, 0.0, 0);
    l2TrkSel.setCuts(-1, -1, -1, -1, 7500, 0.1, 1500, 0.15, 0.30);
    efSel.setCuts(0, 100, 100., 10., -10., 0.);
    break;

  case TrigTauFlags::TAU12:
      l1Sel.setCuts(0, 6000, 0, 0, 0, 0);
      l2CalSel.setCuts(0, 0, 0, 0.2, 0.1, 1., 0);
      l2TrkSel.setCuts(7, -1, 2, 7, 7500, 0.15, 1500, 0.15, 0.30, 2500, true, 0.1, 15000.);
      l2TrkSel.setCoreFromLeading(true);
      efSel.setCuts(1, 8, 0.20, 0.4, 0., 5000., 15000.);
    break;

//   case TrigTauFlags::TAU16I:
//       l1Sel.setCuts(0, 9000, 6000, 0, 0, 0);
//       l2CalSel.setCuts(0, 0, 0, 0.0670, 0.0218, 0.61500, 0);
//       l2TrkSel.setCuts(7, -1, 2, 7, 7500, 0.15, 1500, 0.15, 0.30, 5000, true, 0.1, 20000.);
//       l2TrkSel.setCoreFromLeading(true);
//       efSel.setCuts(1, 8, 0.11, 0.29, 0., 5000., 19000.);
//     break;
    
//   case TrigTauFlags::TAU20I:
//       l1Sel.setCuts(0, 11000, 6000, 0, 0, 0);
//       l2CalSel.setCuts(0, 0, 0, 0.061, 0.01945, 0.6150, 0);
//       l2TrkSel.setCuts(7, -1, 1, 7, 7500, 0.15, 1500, 0.15, 0.30, 5000, true, 0.1, 25000.);
//       l2TrkSel.setCoreFromLeading(true);
//       efSel.setCuts(1, 8, 0.1, 0.27, 0., 7000., 19000.);
//     break;

//   case TrigTauFlags::TAU29I:
//       l1Sel.setCuts(0, 16000, 6000, 0, 0, 0);
//       l2CalSel.setCuts(0, 0, 0, 0.052, 0.0124, 0.545, 0);
//       l2TrkSel.setCuts(7, -1, 1, 7, 7500, 0.15, 1500, 0.15, 0.30, 5000, true, 0.1, 35000.);
//       l2TrkSel.setCoreFromLeading(true);
//       efSel.setCuts(1, 8, 0.10, 0.22, 0., 7000., 27000.);
//     break;

  case TrigTauFlags::TAU38:
      l1Sel.setCuts(0, 21000, 0, 0, 0, 0);
      l2CalSel.setCuts(0, 0, 0, 0.060, 0.0100, 0.60, 0);
      l2TrkSel.setCuts(7, -1, 1, 7, 7500, 0.15, 1500, 0.15, 0.30, 5000, true, 0.2, 45000.);
      l2TrkSel.setCoreFromLeading(true);
      efSel.setCuts(1, 8, 0.14, 0.4, 0., 7000., 35000.);
    break;

//   case TrigTauFlags::TAU38I:
//       l1Sel.setCuts(0, 21000, 6000, 0, 0, 0);
//       l2CalSel.setCuts(0, 0, 0, 0.048, 0.0087, 0.43, 0);
//       l2TrkSel.setCuts(7, -1, 1, 7, 7500, 0.15, 1500, 0.15, 0.30, 5000, true, 0.1, 45000.);
//       l2TrkSel.setCoreFromLeading(true);
//       efSel.setCuts(1, 8, 0.10, 0.22, 0., 7000., 35000.);
//     break;

  case TrigTauFlags::TAU50:
      l1Sel.setCuts(0, 35000, 0, 0, 0, 0);
      l2CalSel.setCuts(0, 0, 0, 0.043, 0.0046, 0.35, 0);
      l2TrkSel.setCuts(7, -1, 1, 7, 7500, 0.15, 1500, 0.15, 0.30, 5000, true, 0.2, 60000.);
      l2TrkSel.setCoreFromLeading(true);
      efSel.setCuts(1, 8, 0.14, 0.4, 0., 7000., 45000.);
    break;

  default :
      cout << ":setTauMenu can not deal with menu " <<  TrigTauFlags::menuName(type)   << endl;
  }

}

//=============================================================================================
void TrigTauSelectionManager::setTauMenu13030pre(TrigTauFlags::TRIGGERMENU type, CUTLEVEL /*level*/) {
  switch (type) {
  case TrigTauFlags::TAUNOCUT: // Not really "no" cuts
    l1Sel.setCuts(0, 5000, 0, 0, 0, 0);
    l2CalSel.setCuts(0, 0, 0, 0.0, 0.0, 0.0, 0);
    l2TrkSel.setCuts(-1, -1, -1, -1, 7500, 0.1, 1500, 0.15, 0.30);
    efSel.setCuts(0, 100, 100., 10., -10., 0.);
    break;


  case TrigTauFlags::TAU12:
      l1Sel.setCuts(0, 6000, 0, 0, 0, 0);
      l2CalSel.setCuts(0, 9700, 0, 0.0616, 0.022, 0.64, 0);
      l2TrkSel.setCuts(7, -1, 2, 7, 7500, 0.15, 1500, 0.15, 0.30, 2500, true, 0.1,0., 1);
      l2TrkSel.setCoreFromLeading(true);
      efSel.setCuts(1, 8, 0.15, 0.4, 0., 6000., 14000.);
    break;

//   case TrigTauFlags::TAU16I:
//       l1Sel.setCuts(0, 9000, 6000, 0, 0, 0);
//       //   Et, EtCalib, nStrips, stripWidth, EMRad, IsoFrac, EMEnWidth2
//       l2CalSel.setCuts(0, 12200, 0, 0.055, 0.0190, 0.66, 0);
//       l2TrkSel.setCuts(7, -1, 2, 7, 7500, 0.15, 1500, 0.15, 0.30, 5000, true, 0.1,0., 1);
//       l2TrkSel.setCoreFromLeading(true);
//       efSel.setCuts(1, 8, 0.11, 0.33, 0., 6000., 19000.);
//     break;
    
//   case TrigTauFlags::TAU20I:
//       l1Sel.setCuts(0, 11000, 6000, 0, 0, 0);
//       l2CalSel.setCuts(0, 17000, 0, 0.051, 0.0160, 0.65, 0);
//       l2TrkSel.setCuts(7, -1, 2, 7, 7500, 0.15, 1500, 0.15, 0.30, 5000, true, 0.1, 0., 1);
//       l2TrkSel.setCoreFromLeading(true);
//       efSel.setCuts(1, 8, 0.1, 0.3, 0., 7000., 22000.);
//     break;

//   case TrigTauFlags::TAU29I:
//       l1Sel.setCuts(0, 16000, 6000, 0, 0, 0);
//       l2CalSel.setCuts(0, 26500, 0, 0.048, 0.015, 0.60, 0);
//       l2TrkSel.setCuts(7, -1, 2, 7, 7500, 0.15, 1500, 0.15, 0.30, 5000, true, 0.1, 0., 1);
//       l2TrkSel.setCoreFromLeading(true);
//       efSel.setCuts(1, 8, 0.09, 0.24, 0., 8000., 31000.);
//     break;

  case TrigTauFlags::TAU38:
      l1Sel.setCuts(0, 25000, 0, 0, 0, 0);
      l2CalSel.setCuts(0, 33500, 0, 0.06, 0.01, 0.6, 0);
      l2TrkSel.setCuts(7, -1, 2, 7, 7500, 0.15, 1500, 0.15, 0.30, 5000, true, 0.2, 0., 1);
      l2TrkSel.setCoreFromLeading(true);
      efSel.setCuts(1, 8, 0.12, 0.3, 0., 8000., 36000.);
    break;

//   case TrigTauFlags::TAU38I:
//       l1Sel.setCuts(0, 25000, 6000, 0, 0, 0);
//       l2CalSel.setCuts(0, 33500, 0, 0.0465, 0.0087, 0.43, 0);
//       l2TrkSel.setCuts(7, -1, 2, 7, 7500, 0.15, 1500, 0.15, 0.30, 5000, true, 0.1, 0., 1);
//       l2TrkSel.setCoreFromLeading(true);
//     break;

  case TrigTauFlags::TAU50:
      l1Sel.setCuts(0, 40000, 0, 0, 0, 0);
      l2CalSel.setCuts(0, 44900, 0, 0.04, 0.00345, 0.35, 0);
      l2TrkSel.setCuts(7, -1, 2, 7, 7500, 0.15, 1500, 0.15, 0.30, 5000, true, 0.2, 0., 1);
      l2TrkSel.setCoreFromLeading(true);
      efSel.setCuts(1, 8, 0.09, 0.24, 0., 8000., 51000.);
    break;

  case TrigTauFlags::TAU84:
      l1Sel.setCuts(0, 40000, 0, 0, 0, 0);
      l2CalSel.setCuts(0, 75000, 0, 0.04, 0.00345, 0.35, 0);
      l2TrkSel.setCuts(7, -1, 2, 7, 7500, 0.15, 1500, 0.15, 0.30, 5000, true, 0.2, 0., 1);
      l2TrkSel.setCoreFromLeading(true);
      efSel.setCuts(1, 8, 0.09, 0.24, 0., 8000., 90000.);
    break;

  default :
      cout << ":setTauMenu can not deal with menu " <<  TrigTauFlags::menuName(type) << endl;
  }

}

//=============================================================================================
void TrigTauSelectionManager::setTauMenu1400(TrigTauFlags::TRIGGERMENU type, CUTLEVEL level) {
  float EFETcut,  L2IDETCut , EFMass;
   switch (type) {
  case TrigTauFlags::TAUNOCUT: // Not really "no" cuts
    l1Sel.setCuts(0, 5000, 0, 0, 0, 0);
    l2CalSel.setCuts(0, 0, 0, 0.0, 0.0, 0.0, 0);
    l2TrkSel.setCuts(-1, -1, -1, -1, 7500, 0.1, 1500, 0.15, 0.30);
    efSel.setCuts(0, 100, 100., 10., -10., 0.);
    break;

  case TrigTauFlags::TAU12:
      l1Sel.setCuts(0, 6000, 0, 0, 0, 0);
      l2TrkSel.setCoreFromLeading(true);
       EFETcut = 12000;
       L2IDETCut = 15000;
       EFMass = 1600;
      //l2CalSel.setCuts(0, 0, 0, 0., 0.022085, 0., 0,5740 );
      l2CalSel.setCuts(0, 0, 0, 1.,0.0163 , 1., 0, 7550);
      switch (level ){
      case SOFT : //95%
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,L2IDETCut, 0);
	efSel.setCuts(1, 5, 0.180, 0, 0., 2000., EFETcut,0,  0.150, 0, 0., 3000., EFETcut,0, 0. );
	break;
      case LOOSE :     //90%
	//l2CalSel.setCuts(0, 0, 0, 0.,0.016325 , 0., 0, 7540);
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,L2IDETCut, 1);
	efSel.setCuts(1, 5, 0.105, 1, 0., 2500., EFETcut,0,  0.125, 1, 0., 3800., EFETcut,0, 0. );
	break;
      case MEDIUM:     //80%
	//l2CalSel.setCuts(0, 0, 0, 0., 0.01353 , 0 , 0, 8380);
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,L2IDETCut*4./3., 1);
	efSel.setCuts(1, 5, 0.075, 0, 0., 2000., EFETcut,0,  0.11, 0, 0., 5000.,EFETcut,0, EFMass );
	break;
      case TIGHT: // 70%
	//l2CalSel.setCuts(0, 0, 0, 0.,0.011555 , 0, 0, 9100);
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,L2IDETCut*4./3., 1);
	efSel.setCuts(1, 4, 0.074, 0, 0., 4000., EFETcut,0,  0.11, 0, 0., 7000., EFETcut,0, EFMass );
	break;
      default :
	cout << "FIXME TrigTauSelectionManager has internal problem with menu " << TrigTauFlags::menuName(type) 
	     << " and level " << level << endl;break;
      }
      break;


  case TrigTauFlags::TAU16:
      l1Sel.setCuts(0, 9000, 0, 0, 0, 0);
      l2TrkSel.setCoreFromLeading(true);
      //l2CalSel.setCuts(0, 0, 0, 0., 0.020105, 0., 0, 8585);
      l2CalSel.setCuts(0, 0, 0, 1., 0.01470, 1., 0, 10400);
       EFMass = 1600;
      switch (level ){
      case LOOSE :     //90%
	//l2CalSel.setCuts(0, 0, 0, 0., 0.014705, 0., 0, 10405);
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,20000, 1);
	efSel.setCuts(1, 5, 0.1, 1, 0., 2500., 16000.,0,  0.12, 1, 0., 2500., 16000.,0, 0. );
	break;
      default :
	cout << "FIXME TrigTauSelectionManager has internal problem with menu " << TrigTauFlags::menuName(type) 
	     << " and level " << level << endl; break;
      }

    break;
    
//   case TrigTauFlags::TAU16I:
//       l1Sel.setCuts(0, 9000, 6000, 0, 0, 0);
//       l2TrkSel.setCoreFromLeading(true);
//       //l2CalSel.setCuts(0, 0, 0, 0., 0.020105, 0., 0, 8585);
//       l2CalSel.setCuts(0, 0, 0, 1., 0.01470, 1., 0, 10400);
//        EFMass = 1600;
//       switch (level ){
//       case SOFT : //95%
// 	//l2CalSel.setCuts(0, 0, 0, 0., 0.020105, 0., 0, 8585);
// 	l2TrkSel.setCuts(7, -1, -1, 7, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,20000., 0);
// 	efSel.setCuts(1, 5, 0.180, 0, 0., 2500., 16000.,0,  0.150, 0, 0., 4000., 16000.,0, 0. );
// 	break;
//       case LOOSE :     //90%
// 	//l2CalSel.setCuts(0, 0, 0, 0., 0.014705, 0., 0, 10405);
// 	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,20000, 1);
// 	efSel.setCuts(1, 5, 0.1, 1, 0., 2500., 16000.,0,  0.12, 1, 0., 2500., 16000.,0, 0. );
// 	break;
//       case MEDIUM:     //85%
// 	//l2CalSel.setCuts(0, 0, 0, 0., 0.012185, 0 , 0,11575 );
// 	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,26667., 1);
// 	efSel.setCuts(1, 5, 0.078, 1, 0., 2500., 16000.,0,  0.11, 1, 0., 7000., 16000.,0, EFMass );
// 	break;
//       case TIGHT: // 80%
// 	//l2CalSel.setCuts(0, 0, 0, 0.,0.010565 , 0, 0, 12355);
// 	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,26667., 1);
// 	efSel.setCuts(1, 4, 0.074, 1, 0., 4500., 16000.,0,  0.10, 1, 0., 9000., 16000.,0, EFMass );
// 	break;
//       default :
// 	cout << "FIXME TrigTauSelectionManager has internal problem with menu " << TrigTauFlags::menuName(type) 
// 	     << " and level " << level << endl; break;
//       }

//     break;
    
//   case TrigTauFlags::TAU20I:
//       l1Sel.setCuts(0, 11000, 6000, 0, 0, 0);
//       l2TrkSel.setCuts(7, -1, 2, 7, 7500, 0.15, 1500, 0.15, 0.30, 5000, true, 0.1, 0., 1);
//       l2TrkSel.setCoreFromLeading(true);
//       efSel.setCuts(1, 8, 0.1, 0.3, 0., 7000., 22000.);
//       l2CalSel.setCuts(0, 0, 0, 1., 0.0136, 1., 0, 13000);
//        EFMass = 1600;
//       switch (level ){
//       case SOFT : //95%
// 	//l2CalSel.setCuts(0, 0, 0, 0., 0.018575, 0., 0, 10750);
// 	l2TrkSel.setCuts(7, -1, -1, 7, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,25000., 0);
// 	efSel.setCuts(1, 5, 0.180, 0, 0., 2500., 20000.,0,  0.150, 0, 0., 4000., 20000.,0, 0.  );
// 	break;
//       case LOOSE :     //90%
// 	//l2CalSel.setCuts(0, 0, 0, 0., 0.013625, 0., 0, 12990);
// 	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,25000, 1);
// 	efSel.setCuts(1, 5, 0.100, 1, 0., 2500., 20000.,0,  0.12, 1, 0., 4500., 20000.,0 , 0. );
// 	break;
//       case MEDIUM:     //80%
// 	//l2CalSel.setCuts(0, 0, 0, 0., 0.011375 , 0 , 0, 14250);
// 	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,33000., 1);
// 	efSel.setCuts(1, 5, 0.078, 1, 0., 3500., 20000.,0,  0.11, 1, 0., 8000., 20000.,0, EFMass  );
// 	break;
//       case TIGHT: // 70%
// 	//l2CalSel.setCuts(0, 0, 0, 0.,0.009845 , 0, 0, 15090);
// 	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,33000., 1);
// 	efSel.setCuts(1, 4, 0.073, 1, 0., 5500., 20000.,0,  0.1, 1, 0., 10000., 20000.,0, EFMass  );
// 	break;
//       default :
// 	cout << "FIXME TrigTauSelectionManager has internal problem with menu " << TrigTauFlags::menuName(type) 
// 	     << " and level " << level << endl;break;
//       }
//     break;

  case TrigTauFlags::TAU29:
//   case TrigTauFlags::TAU29I:
//       if( type ==  TrigTauFlags::TAU29I)   l1Sel.setCuts(0, 16000, 6000, 0, 0, 0);
//       else                                 
      l1Sel.setCuts(0, 16000, 0, 0, 0, 0);
      
      //l2CalSel.setCuts(0, 26500, 0, 0.048, 0.015, 0.60, 0);
      //l2TrkSel.setCuts(7, -1, 2, 7, 7500, 0.15, 1500, 0.15, 0.30, 5000, true, 0.1, 0., 1);
      l2TrkSel.setCoreFromLeading(true);
      //efSel.setCuts(1, 8, 0.09, 0.24, 0., 8000., 31000.);
      //l2CalSel.setCuts(0, 0, 0, 0., 0.015155, 0., 0, 16040);
      l2CalSel.setCuts(0, 0, 0, 0., 0.0115, 0., 0, 18700);
       EFMass = 1600;
       EFETcut = 29000;
      switch (level ){
      case SOFT : //95%
	//l2CalSel.setCuts(0, 0, 0, 0., 0.015155, 0., 0, 16040);
	l2TrkSel.setCuts(7, -1, -1, 7, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,35000., 0);
	efSel.setCuts(1, 5, 0.180, 0, 0., 2500., EFETcut,0,  0.150, 0, 0., 4000., EFETcut,0 , 0. );
	break;
      case LOOSE :     //90%
	//l2CalSel.setCuts(0, 0, 0, 0., 0.011465, 0., 0, 18760);
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,35000, 1);
	efSel.setCuts(1, 5, 0.100, 1, 0., 3000., EFETcut,0,  0.12, 1, 0., 5000., EFETcut,0, 0.  );
	break;
      case MEDIUM:     //80%
	//l2CalSel.setCuts(0, 0, 0, 0., 0.009575 , 0 , 0,20360 );
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,46700., 1);
	efSel.setCuts(1, 5, 0.078, 1, 0., 5000., EFETcut,0,  0.11, 1, 0., 10000., EFETcut,0 , EFMass );
	break;
      case TIGHT: // 70%
	//l2CalSel.setCuts(0, 0, 0, 0.,0.008225 , 0, 0, 21800);
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,46700., 1);
	efSel.setCuts(1, 4, 0.073, 1, 0., 8000., EFETcut,0,  0.1, 1, 0., 13000., EFETcut,0 , EFMass );
	break;
      default :
	cout << "FIXME TrigTauSelectionManager has internal problem with menu " << TrigTauFlags::menuName(type) 
	     << " and level " << level << endl;break;
      }
    break;

  case TrigTauFlags::TAU38:
      l1Sel.setCuts(0, 25000, 0, 0, 0, 0);
      l2TrkSel.setCoreFromLeading(true);
       EFETcut = 38000;
       L2IDETCut = 45000;
      //l2CalSel.setCuts(0, 0, 0, 0., 0.012185, 0., 0, 22630);
      l2CalSel.setCuts(0, 0, 0, 0.,0.009 , 0., 0, 25800);
       EFMass = 1600;
      switch (level ){
      case SOFT : //95%
	//l2CalSel.setCuts(0, 0, 0, 0., 0.012185, 0., 0, 22630);
	l2TrkSel.setCuts(7, -1, -1, 7, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,L2IDETCut, 0);
	efSel.setCuts(1, 5, 0.180, 0, 0., 2500., EFETcut,0,  0.150, 0, 0., 4000., EFETcut,0, 0.  );
	break;
      case LOOSE :     //90%
	//l2CalSel.setCuts(0, 0, 0, 0.,0.008945 , 0., 0, 25870);
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,L2IDETCut, 1);
	efSel.setCuts(1, 5, 0.100, 1, 0., 3800., EFETcut,0,  0.120, 1, 0., 5500., EFETcut,0, 0.  );
	break;
      case MEDIUM:     //80%
	//l2CalSel.setCuts(0, 0, 0, 0., 0.007415 , 0 , 0, 27490);
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,L2IDETCut*4./3., 1);
	efSel.setCuts(1, 5, 0.078, 1, 0., 7000., EFETcut,0,  0.11, 1, 0., 12000.,EFETcut,0 , EFMass );
	break;
      case TIGHT: // 70%
	//l2CalSel.setCuts(0, 0, 0, 0.,0.006335 , 0, 0,29110 );
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,L2IDETCut*4./3., 1);
	efSel.setCuts(1, 4, 0.072, 1, 0., 11000., EFETcut,0,  0.090, 1, 0., 15000., EFETcut,0, EFMass  );
	break;
      default :
	cout << "FIXME TrigTauSelectionManager has internal problem with menu " << TrigTauFlags::menuName(type) 
	     << " and level " << level << endl;break;
      }
    break;

//   case TrigTauFlags::TAU38I:
//       l1Sel.setCuts(0, 25000, 6000, 0, 0, 0);
//       l2TrkSel.setCoreFromLeading(true);
//        EFETcut = 38000;
//       l2CalSel.setCuts(0, 0, 0, 0.,0.009 , 0., 0, 25800);
//        L2IDETCut = 45000;
//        EFMass = 1600;
//       switch (level ){
//       case SOFT : //95%
// 	//l2CalSel.setCuts(0, 0, 0, 0., 0.012185, 0., 0, 22630);
// 	l2TrkSel.setCuts(7, -1, -1, 7, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,L2IDETCut, 0);
// 	efSel.setCuts(1, 5, 0.180, 0, 0., 2500., EFETcut,0,  0.150, 0, 0., 4000., EFETcut,0, 0  );
// 	break;
//       case LOOSE :     //90%
// 	//l2CalSel.setCuts(0, 0, 0, 0.,0.008945 , 0., 0, 25870);
// 	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,L2IDETCut, 1);
// 	efSel.setCuts(1, 5, 0.100, 1, 0., 3800., EFETcut,0,  0.120, 1, 0., 5500., EFETcut,0, 0.  );
// 	break;
//       case MEDIUM:     //80%
// 	//l2CalSel.setCuts(0, 0, 0, 0., 0.007415 , 0 , 0, 27490);
// 	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,L2IDETCut*4./3., 1);
// 	efSel.setCuts(1, 5, 0.078, 0, 0., 7000., EFETcut,0,  0.11, 0, 0., 12000.,EFETcut,0 , EFMass );
// 	break;
//       case TIGHT: // 70%
// 	//l2CalSel.setCuts(0, 0, 0, 0.,0.006335 , 0, 0,29110 );
// 	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,L2IDETCut*4./3., 1);
// 	efSel.setCuts(1, 4, 0.072, 0, 0., 11000., EFETcut,0,  0.090, 0, 0., 15000., EFETcut,0, EFMass  );
// 	break;      
//       default :
// 	cout << "FIXME TrigTauSelectionManager has internal problem with menu " << TrigTauFlags::menuName(type) 
// 	     << " and level " << level << endl;break;
//       }
//     break;

  case TrigTauFlags::TAU50:
      l1Sel.setCuts(0, 40000, 0, 0, 0, 0);
      l2TrkSel.setCoreFromLeading(true);
      //l2CalSel.setCuts(0, 0, 0, 0., 0.009125, 0., 0, 31645);
      l2CalSel.setCuts(0, 0, 0, 0., 0.0049, 0., 0, 36000);
       EFMass = 1600;
       L2IDETCut = 60000;
       EFETcut= 50000;
      switch (level ){
      case SOFT : //95%
	//l2CalSel.setCuts(0, 0, 0, 0., 0.012185, 0., 0, 22630);
	l2TrkSel.setCuts(7, -1, -1, 7, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,L2IDETCut, 0);
	efSel.setCuts(1, 5, 0.180, 0, 0., 6000., EFETcut,0,  0.180, 0, 0., 6000., EFETcut,0, 0.  );
	break;
      case LOOSE :     //90%
	//l2CalSel.setCuts(0, 0, 0, 0., 0.004895, 0., 0, 36055);
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,L2IDETCut, 1);
	efSel.setCuts(1, 5, 0.090, 1, 0., 10000., EFETcut,0,  0.090, 1, 0., 10000., EFETcut,0, 0.  );
	break;
      case MEDIUM:     //80%
	//l2CalSel.setCuts(0, 0, 0, 0., 0.003725 , 0 , 0, 38995);
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,L2IDETCut*4./3., 1);
	efSel.setCuts(1, 5, 0.060, 1, 0., 17000., EFETcut,0,  0.068, 1, 0., 15000.,EFETcut,0, EFMass  );
	break;
      case TIGHT: // 70%
	///l2CalSel.setCuts(0, 0, 0, 0.,0.003095 , 0, 0, 41095);
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,L2IDETCut*4./3., 1);
	efSel.setCuts(1, 4, 0.050, 1, 0., 21000., EFETcut,0,  0.065, 1, 0., 25000., EFETcut,0 , EFMass );
	break;
      default :
	cout << "FIXME TrigTauSelectionManager has internal problem with menu " << TrigTauFlags::menuName(type) 
	     << " and level " << level << endl;break;
      }
    break;

  case TrigTauFlags::TAU84:
      l1Sel.setCuts(0, 40000, 0, 0, 0, 0);
      l2CalSel.setCuts(0, 75000, 0, 0.04, 0.00345, 0.35, 0);
      l2TrkSel.setCuts(7, -1, 2, 7, 7500, 0.15, 1500, 0.15, 0.30, 5000, true, 0.2, 0., 1);
      l2TrkSel.setCoreFromLeading(true);
      efSel.setCuts(1, 8, 0.09, 0.24, 0., 8000., 90000.);
       EFETcut = 84000;
       L2IDETCut = 100000;
      //l2CalSel.setCuts(0, 0, 0, 0., 0.007685, 0., 0, 47345);
      l2CalSel.setCuts(0, 0, 0, 1.,0.0042 , 1., 0,56000 );
       EFMass=1600;
      switch (level ){
      case SOFT : //95%
	//l2CalSel.setCuts(0, 0, 0, 0., 0.007685, 0., 0, 47345);
	l2TrkSel.setCuts(7, -1, -1, 7, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,L2IDETCut, 0);
	efSel.setCuts(1, 5, 0.180, 0, 0., 7000., EFETcut,0,  0.140, 0, 0., 9000., EFETcut,0, 0.  );
	break;
      case LOOSE :     //90%
	//l2CalSel.setCuts(0, 0, 0, 0.,0.004175 , 0., 0,56045 );
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,L2IDETCut, 1);
	efSel.setCuts(1, 5, 0.080, 1, 0., 12000., EFETcut,0,  0.085, 1, 0., 13000., EFETcut,0 , 0. );
	break;
      case MEDIUM:     //80%
	//l2CalSel.setCuts(0, 0, 0, 0., 0.003275 , 0 , 0,60105 );
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,L2IDETCut*4./3., 1);
	efSel.setCuts(1, 5, 0.060, 1, 0., 25000., EFETcut,0,  0.068, 1, 0., 25000.,EFETcut,0 , EFMass );
	break;
      case TIGHT: // 70%
	//l2CalSel.setCuts(0, 0, 0, 0.,0.002735 , 0, 0, 63585);
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,L2IDETCut*4./3., 1);
	efSel.setCuts(1, 4, 0.046, 1, 0., 30000., EFETcut,0,  0.055, 1, 0., 31000., EFETcut,0 , EFMass );
	break;
      default :
	cout << "FIXME TrigTauSelectionManager has internal problem with menu " << TrigTauFlags::menuName(type) 
	     << " and level " << level << endl;break;
      }
    break;

  case TrigTauFlags::TAU125:
      l1Sel.setCuts(0, 40000, 0, 0, 0, 0);
      l2TrkSel.setCoreFromLeading(true);
       EFETcut =125000;
       L2IDETCut = 150000;
      l2CalSel.setCuts(0, 0, 0, 1.,0.0034 , 1., 0, 93000.);
       EFMass=1600;
      switch (level ){
      case SOFT : //95%
	//l2CalSel.setCuts(0, 0, 0, 0., 0.007685, 0., 0, 47345);
	l2TrkSel.setCuts(7, -1, -1, 7, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,L2IDETCut, 0);
	efSel.setCuts(1, 5, 0.180, 0, 0., 7000., EFETcut,0,  0.140, 0, 0., 9000., EFETcut,0, 0.  );
	break;
      case LOOSE :     //90%
	//l2CalSel.setCuts(0, 0, 0, 0.,0.004175 , 0., 0,56045 );
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,L2IDETCut, 1);
	efSel.setCuts(1, 5, 0.07, 1, 0., 15000., EFETcut,0,  0.08, 1, 0., 16000., EFETcut,0 , 0. );
	break;
      case MEDIUM:     //80%
	//l2CalSel.setCuts(0, 0, 0, 0., 0.003275 , 0 , 0,60105 );
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,L2IDETCut*4./3., 1);
	efSel.setCuts(1, 5, 0.058, 1, 0., 32000., EFETcut,0,  0.055, 1, 0., 22000.,EFETcut,0 , EFMass );
	break;
      case TIGHT: // 70%
	//l2CalSel.setCuts(0, 0, 0, 0.,0.002735 , 0, 0, 63585);
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,L2IDETCut*4./3., 1);
	efSel.setCuts(1, 4, 0.043, 1, 0., 41000., EFETcut,0,  0.050, 1, 0., 36000., EFETcut,0 , EFMass );
	break;
      default :
	cout << "FIXME TrigTauSelectionManager has internal problem with menu " << TrigTauFlags::menuName(type) 
	     << " and level " << level << endl;break;
      }
      break;
   default :
     cout << ":setTauMenu MENU1400 can not deal with menu " << TrigTauFlags::menuName(type)  << endl;
     break;
   }


}

//=============================================================================================
void TrigTauSelectionManager::setTauMenu1520test(TrigTauFlags::TRIGGERMENU type, CUTLEVEL level) {
  float EFETcut,  L2IDETCut , EFMass;
   switch (type) {
  case TrigTauFlags::TAUNOCUT: // Not really "no" cuts
    l1Sel.setCuts(0, 4000, 0, 0, 0, 0);
    l2CalSel.setCuts(0, 0, 0, 0.0, 0.0, 0.0, 0);
    l2TrkSel.setCuts(-1, -1, -1, -1, 7500, 0.1, 1500, 0.15, 0.30);
    efSel.setCuts(0, 100, 100., 10., -10., 0.);
    break;

  case TrigTauFlags::TAU12:
      l1Sel.setCuts(0, 5000, 0, 0, 0, 0);
      l2TrkSel.setCoreFromLeading(true);
       EFETcut = 12000;
       L2IDETCut = 15000;
       EFMass = 1600;
      //l2CalSel.setCuts(0, 0, 0, 0., 0.022085, 0., 0,5740 );
      l2CalSel.setCuts(0, 0, 0, 1.,0.0163 , 1., 0, 7550);
      switch (level ){
      case SOFT : //95%
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,L2IDETCut, 0);
	efSel.setCuts(1, 5, 0.180, 0, 0., 2000., EFETcut,0,  0.150, 0, 0., 3000., EFETcut,0, 0. );
	break;
      case LOOSE :     //90%
	//l2CalSel.setCuts(0, 0, 0, 0.,0.016325 , 0., 0, 7540);
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,L2IDETCut, 1);
	efSel.setCuts(1, 5, 0.105, 1, 0., 2500., EFETcut,0,  0.125, 1, 0., 3800., EFETcut,0, 0. );
	break;
      case MEDIUM:     //80%
	//l2CalSel.setCuts(0, 0, 0, 0., 0.01353 , 0 , 0, 8380);
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,L2IDETCut*4./3., 1);
	efSel.setCuts(1, 5, 0.075, 0, 0., 2000., EFETcut,0,  0.11, 0, 0., 5000.,EFETcut,0, EFMass );
	break;
      case TIGHT: // 70%
	//l2CalSel.setCuts(0, 0, 0, 0.,0.011555 , 0, 0, 9100);
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,L2IDETCut*4./3., 1);
	efSel.setCuts(1, 4, 0.074, 0, 0., 4000., EFETcut,0,  0.11, 0, 0., 7000., EFETcut,0, EFMass );
	break;
      default :
	cout << "FIXME TrigTauSelectionManager has internal problem with menu " << TrigTauFlags::menuName(type) 
	     << " and level " << level << endl;break;
      }
      break;


  case TrigTauFlags::TAU16:
      l1Sel.setCuts(0, 6000, 0, 0, 0, 0);
      l2TrkSel.setCoreFromLeading(true);
      //l2CalSel.setCuts(0, 0, 0, 0., 0.020105, 0., 0, 8585);
      l2CalSel.setCuts(0, 0, 0, 1., 0.01470, 1., 0, 10400);
       EFMass = 1600;
      switch (level ){
      case LOOSE :     //90%
	//l2CalSel.setCuts(0, 0, 0, 0., 0.014705, 0., 0, 10405);
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,20000, 1);
	efSel.setCuts(1, 5, 0.1, 1, 0., 2500., 16000.,0,  0.12, 1, 0., 2500., 16000.,0, 0. );
	break;
      default :
	cout << "FIXME TrigTauSelectionManager has internal problem with menu " << TrigTauFlags::menuName(type) 
	     << " and level " << level << endl; break;
      }

    break;
    
//   case TrigTauFlags::TAU16I:
//       l1Sel.setCuts(0, 6000, 4000, 0, 0, 0);
//       l2TrkSel.setCoreFromLeading(true);
//       //l2CalSel.setCuts(0, 0, 0, 0., 0.020105, 0., 0, 8585);
//       l2CalSel.setCuts(0, 0, 0, 1., 0.01470, 1., 0, 10400);
//        EFMass = 1600;
//       switch (level ){
//       case SOFT : //95%
// 	//l2CalSel.setCuts(0, 0, 0, 0., 0.020105, 0., 0, 8585);
// 	l2TrkSel.setCuts(7, -1, -1, 7, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,20000., 0);
// 	efSel.setCuts(1, 5, 0.180, 0, 0., 2500., 16000.,0,  0.150, 0, 0., 4000., 16000.,0, 0. );
// 	break;
//       case LOOSE :     //90%
// 	//l2CalSel.setCuts(0, 0, 0, 0., 0.014705, 0., 0, 10405);
// 	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,20000, 1);
// 	efSel.setCuts(1, 5, 0.1, 1, 0., 2500., 16000.,0,  0.12, 1, 0., 2500., 16000.,0, 0. );
// 	break;
//       case MEDIUM:     //85%
// 	//l2CalSel.setCuts(0, 0, 0, 0., 0.012185, 0 , 0,11575 );
// 	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,26667., 1);
// 	efSel.setCuts(1, 5, 0.078, 1, 0., 2500., 16000.,0,  0.11, 1, 0., 7000., 16000.,0, EFMass );
// 	break;
//       case TIGHT: // 80%
// 	//l2CalSel.setCuts(0, 0, 0, 0.,0.010565 , 0, 0, 12355);
// 	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,26667., 1);
// 	efSel.setCuts(1, 4, 0.074, 1, 0., 4500., 16000.,0,  0.10, 1, 0., 9000., 16000.,0, EFMass );
// 	break;
//       default :
// 	cout << "FIXME TrigTauSelectionManager has internal problem with menu " << TrigTauFlags::menuName(type) 
// 	     << " and level " << level << endl; break;
//       }

//     break;
    
//   case TrigTauFlags::TAU20I:
//       l1Sel.setCuts(0, 8000, 4000, 0, 0, 0);
//       l2TrkSel.setCuts(7, -1, 2, 7, 7500, 0.15, 1500, 0.15, 0.30, 5000, true, 0.1, 0., 1);
//       l2TrkSel.setCoreFromLeading(true);
//       efSel.setCuts(1, 8, 0.1, 0.3, 0., 7000., 22000.);
//       l2CalSel.setCuts(0, 0, 0, 1., 0.0136, 1., 0, 13000);
//        EFMass = 1600;
//       switch (level ){
//       case SOFT : //95%
// 	//l2CalSel.setCuts(0, 0, 0, 0., 0.018575, 0., 0, 10750);
// 	l2TrkSel.setCuts(7, -1, -1, 7, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,25000., 0);
// 	efSel.setCuts(1, 5, 0.180, 0, 0., 2500., 20000.,0,  0.150, 0, 0., 4000., 20000.,0, 0.  );
// 	break;
//       case LOOSE :     //90%
// 	//l2CalSel.setCuts(0, 0, 0, 0., 0.013625, 0., 0, 12990);
// 	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,25000, 1);
// 	efSel.setCuts(1, 5, 0.100, 1, 0., 2500., 20000.,0,  0.12, 1, 0., 4500., 20000.,0 , 0. );
// 	break;
//       case MEDIUM:     //80%
// 	//l2CalSel.setCuts(0, 0, 0, 0., 0.011375 , 0 , 0, 14250);
// 	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,33000., 1);
// 	efSel.setCuts(1, 5, 0.078, 1, 0., 3500., 20000.,0,  0.11, 1, 0., 8000., 20000.,0, EFMass  );
// 	break;
//       case TIGHT: // 70%
// 	//l2CalSel.setCuts(0, 0, 0, 0.,0.009845 , 0, 0, 15090);
// 	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,33000., 1);
// 	efSel.setCuts(1, 4, 0.073, 1, 0., 5500., 20000.,0,  0.1, 1, 0., 10000., 20000.,0, EFMass  );
// 	break;
//       default :
// 	cout << "FIXME TrigTauSelectionManager has internal problem with menu " << TrigTauFlags::menuName(type) 
// 	     << " and level " << level << endl;break;
//       }
//     break;

  case TrigTauFlags::TAU29:
//   case TrigTauFlags::TAU29I:
//       if( type ==  TrigTauFlags::TAU29I)   l1Sel.setCuts(0, 11000, 4000, 0, 0, 0);
//       else                                
      l1Sel.setCuts(0, 11000, 0, 0, 0, 0);
      //l2CalSel.setCuts(0, 26500, 0, 0.048, 0.015, 0.60, 0);
      //l2TrkSel.setCuts(7, -1, 2, 7, 7500, 0.15, 1500, 0.15, 0.30, 5000, true, 0.1, 0., 1);
      l2TrkSel.setCoreFromLeading(true);
      //efSel.setCuts(1, 8, 0.09, 0.24, 0., 8000., 31000.);
      //l2CalSel.setCuts(0, 0, 0, 0., 0.015155, 0., 0, 16040);
      l2CalSel.setCuts(0, 0, 0, 0., 0.0115, 0., 0, 18700);
       EFMass = 1600;
       EFETcut = 29000;
      switch (level ){
      case SOFT : //95%
	//l2CalSel.setCuts(0, 0, 0, 0., 0.015155, 0., 0, 16040);
	l2TrkSel.setCuts(7, -1, -1, 7, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,35000., 0);
	efSel.setCuts(1, 5, 0.180, 0, 0., 2500., EFETcut,0,  0.150, 0, 0., 4000., EFETcut,0 , 0. );
	break;
      case LOOSE :     //90%
	//l2CalSel.setCuts(0, 0, 0, 0., 0.011465, 0., 0, 18760);
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,35000, 1);
	efSel.setCuts(1, 5, 0.100, 1, 0., 3000., EFETcut,0,  0.12, 1, 0., 5000., EFETcut,0, 0.  );
	break;
      case MEDIUM:     //80%
	//l2CalSel.setCuts(0, 0, 0, 0., 0.009575 , 0 , 0,20360 );
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,46700., 1);
	efSel.setCuts(1, 5, 0.078, 1, 0., 5000., EFETcut,0,  0.11, 1, 0., 10000., EFETcut,0 , EFMass );
	break;
      case TIGHT: // 70%
	//l2CalSel.setCuts(0, 0, 0, 0.,0.008225 , 0, 0, 21800);
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,46700., 1);
	efSel.setCuts(1, 4, 0.073, 1, 0., 8000., EFETcut,0,  0.1, 1, 0., 13000., EFETcut,0 , EFMass );
	break;
      default :
	cout << "FIXME TrigTauSelectionManager has internal problem with menu " << TrigTauFlags::menuName(type) 
	     << " and level " << level << endl;break;
      }
    break;

  case TrigTauFlags::TAU38:
      l1Sel.setCuts(0, 20000, 0, 0, 0, 0);
      l2TrkSel.setCoreFromLeading(true);
       EFETcut = 38000;
       L2IDETCut = 45000;
      //l2CalSel.setCuts(0, 0, 0, 0., 0.012185, 0., 0, 22630);
      l2CalSel.setCuts(0, 0, 0, 0.,0.009 , 0., 0, 25800);
       EFMass = 1600;
      switch (level ){
      case SOFT : //95%
	//l2CalSel.setCuts(0, 0, 0, 0., 0.012185, 0., 0, 22630);
	l2TrkSel.setCuts(7, -1, -1, 7, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,L2IDETCut, 0);
	efSel.setCuts(1, 5, 0.180, 0, 0., 2500., EFETcut,0,  0.150, 0, 0., 4000., EFETcut,0, 0.  );
	break;
      case LOOSE :     //90%
	//l2CalSel.setCuts(0, 0, 0, 0.,0.008945 , 0., 0, 25870);
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,L2IDETCut, 1);
	efSel.setCuts(1, 5, 0.100, 1, 0., 3800., EFETcut,0,  0.120, 1, 0., 5500., EFETcut,0, 0.  );
	break;
      case MEDIUM:     //80%
	//l2CalSel.setCuts(0, 0, 0, 0., 0.007415 , 0 , 0, 27490);
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,L2IDETCut*4./3., 1);
	efSel.setCuts(1, 5, 0.078, 1, 0., 7000., EFETcut,0,  0.11, 1, 0., 12000.,EFETcut,0 , EFMass );
	break;
      case TIGHT: // 70%
	//l2CalSel.setCuts(0, 0, 0, 0.,0.006335 , 0, 0,29110 );
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,L2IDETCut*4./3., 1);
	efSel.setCuts(1, 4, 0.072, 1, 0., 11000., EFETcut,0,  0.090, 1, 0., 15000., EFETcut,0, EFMass  );
	break;
      default :
	cout << "FIXME TrigTauSelectionManager has internal problem with menu " << TrigTauFlags::menuName(type) 
	     << " and level " << level << endl;break;
      }
    break;

//   case TrigTauFlags::TAU38I:
//       l1Sel.setCuts(0, 20000, 4000, 0, 0, 0);
//       l2TrkSel.setCoreFromLeading(true);
//        EFETcut = 38000;
//       l2CalSel.setCuts(0, 0, 0, 0.,0.009 , 0., 0, 25800);
//        L2IDETCut = 45000;
//        EFMass = 1600;
//       switch (level ){
//       case SOFT : //95%
// 	//l2CalSel.setCuts(0, 0, 0, 0., 0.012185, 0., 0, 22630);
// 	l2TrkSel.setCuts(7, -1, -1, 7, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,L2IDETCut, 0);
// 	efSel.setCuts(1, 5, 0.180, 0, 0., 2500., EFETcut,0,  0.150, 0, 0., 4000., EFETcut,0, 0  );
// 	break;
//       case LOOSE :     //90%
// 	//l2CalSel.setCuts(0, 0, 0, 0.,0.008945 , 0., 0, 25870);
// 	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,L2IDETCut, 1);
// 	efSel.setCuts(1, 5, 0.100, 1, 0., 3800., EFETcut,0,  0.120, 1, 0., 5500., EFETcut,0, 0.  );
// 	break;
//       case MEDIUM:     //80%
// 	//l2CalSel.setCuts(0, 0, 0, 0., 0.007415 , 0 , 0, 27490);
// 	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,L2IDETCut*4./3., 1);
// 	efSel.setCuts(1, 5, 0.078, 0, 0., 7000., EFETcut,0,  0.11, 0, 0., 12000.,EFETcut,0 , EFMass );
// 	break;
//       case TIGHT: // 70%
// 	//l2CalSel.setCuts(0, 0, 0, 0.,0.006335 , 0, 0,29110 );
// 	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,L2IDETCut*4./3., 1);
// 	efSel.setCuts(1, 4, 0.072, 0, 0., 11000., EFETcut,0,  0.090, 0, 0., 15000., EFETcut,0, EFMass  );
// 	break;      
//       default :
// 	cout << "FIXME TrigTauSelectionManager has internal problem with menu " << TrigTauFlags::menuName(type) 
// 	     << " and level " << level << endl;break;
//       }
//     break;

  case TrigTauFlags::TAU50:
      l1Sel.setCuts(0, 30000, 0, 0, 0, 0);
      l2TrkSel.setCoreFromLeading(true);
      //l2CalSel.setCuts(0, 0, 0, 0., 0.009125, 0., 0, 31645);
      l2CalSel.setCuts(0, 0, 0, 0., 0.0049, 0., 0, 36000);
       EFMass = 1600;
       L2IDETCut = 60000;
       EFETcut= 50000;
      switch (level ){
      case SOFT : //95%
	//l2CalSel.setCuts(0, 0, 0, 0., 0.012185, 0., 0, 22630);
	l2TrkSel.setCuts(7, -1, -1, 7, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,L2IDETCut, 0);
	efSel.setCuts(1, 5, 0.180, 0, 0., 6000., EFETcut,0,  0.180, 0, 0., 6000., EFETcut,0, 0.  );
	break;
      case LOOSE :     //90%
	//l2CalSel.setCuts(0, 0, 0, 0., 0.004895, 0., 0, 36055);
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,L2IDETCut, 1);
	efSel.setCuts(1, 5, 0.090, 1, 0., 10000., EFETcut,0,  0.090, 1, 0., 10000., EFETcut,0, 0.  );
	break;
      case MEDIUM:     //80%
	//l2CalSel.setCuts(0, 0, 0, 0., 0.003725 , 0 , 0, 38995);
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,L2IDETCut*4./3., 1);
	efSel.setCuts(1, 5, 0.060, 1, 0., 17000., EFETcut,0,  0.068, 1, 0., 15000.,EFETcut,0, EFMass  );
	break;
      case TIGHT: // 70%
	///l2CalSel.setCuts(0, 0, 0, 0.,0.003095 , 0, 0, 41095);
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,L2IDETCut*4./3., 1);
	efSel.setCuts(1, 4, 0.050, 1, 0., 21000., EFETcut,0,  0.065, 1, 0., 25000., EFETcut,0 , EFMass );
	break;
      default :
	cout << "FIXME TrigTauSelectionManager has internal problem with menu " << TrigTauFlags::menuName(type) 
	     << " and level " << level << endl;break;
      }
    break;

  case TrigTauFlags::TAU84:
      l1Sel.setCuts(0, 30000, 0, 0, 0, 0);
      l2CalSel.setCuts(0, 75000, 0, 0.04, 0.00345, 0.35, 0);
      l2TrkSel.setCuts(7, -1, 2, 7, 7500, 0.15, 1500, 0.15, 0.30, 5000, true, 0.2, 0., 1);
      l2TrkSel.setCoreFromLeading(true);
      efSel.setCuts(1, 8, 0.09, 0.24, 0., 8000., 90000.);
       EFETcut = 84000;
       L2IDETCut = 100000;
      //l2CalSel.setCuts(0, 0, 0, 0., 0.007685, 0., 0, 47345);
      l2CalSel.setCuts(0, 0, 0, 1.,0.0042 , 1., 0,56000 );
       EFMass=1600;
      switch (level ){
      case SOFT : //95%
	//l2CalSel.setCuts(0, 0, 0, 0., 0.007685, 0., 0, 47345);
	l2TrkSel.setCuts(7, -1, -1, 7, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,L2IDETCut, 0);
	efSel.setCuts(1, 5, 0.180, 0, 0., 7000., EFETcut,0,  0.140, 0, 0., 9000., EFETcut,0, 0.  );
	break;
      case LOOSE :     //90%
	//l2CalSel.setCuts(0, 0, 0, 0.,0.004175 , 0., 0,56045 );
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,L2IDETCut, 1);
	efSel.setCuts(1, 5, 0.080, 1, 0., 12000., EFETcut,0,  0.085, 1, 0., 13000., EFETcut,0 , 0. );
	break;
      case MEDIUM:     //80%
	//l2CalSel.setCuts(0, 0, 0, 0., 0.003275 , 0 , 0,60105 );
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,L2IDETCut*4./3., 1);
	efSel.setCuts(1, 5, 0.060, 1, 0., 25000., EFETcut,0,  0.068, 1, 0., 25000.,EFETcut,0 , EFMass );
	break;
      case TIGHT: // 70%
	//l2CalSel.setCuts(0, 0, 0, 0.,0.002735 , 0, 0, 63585);
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,L2IDETCut*4./3., 1);
	efSel.setCuts(1, 4, 0.046, 1, 0., 30000., EFETcut,0,  0.055, 1, 0., 31000., EFETcut,0 , EFMass );
	break;
      default :
	cout << "FIXME TrigTauSelectionManager has internal problem with menu " << TrigTauFlags::menuName(type) 
	     << " and level " << level << endl;break;
      }
    break;

  case TrigTauFlags::TAU125:
      l1Sel.setCuts(0, 30000, 0, 0, 0, 0);
      l2TrkSel.setCoreFromLeading(true);
       EFETcut =125000;
       L2IDETCut = 150000;
      l2CalSel.setCuts(0, 0, 0, 1.,0.0034 , 1., 0, 93000.);
       EFMass=1600;
      switch (level ){
      case SOFT : //95%
	//l2CalSel.setCuts(0, 0, 0, 0., 0.007685, 0., 0, 47345);
	l2TrkSel.setCuts(7, -1, -1, 7, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,L2IDETCut, 0);
	efSel.setCuts(1, 5, 0.180, 0, 0., 7000., EFETcut,0,  0.140, 0, 0., 9000., EFETcut,0, 0.  );
	break;
      case LOOSE :     //90%
	//l2CalSel.setCuts(0, 0, 0, 0.,0.004175 , 0., 0,56045 );
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,L2IDETCut, 1);
	efSel.setCuts(1, 5, 0.07, 1, 0., 15000., EFETcut,0,  0.08, 1, 0., 16000., EFETcut,0 , 0. );
	break;
      case MEDIUM:     //80%
	//l2CalSel.setCuts(0, 0, 0, 0., 0.003275 , 0 , 0,60105 );
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,L2IDETCut*4./3., 1);
	efSel.setCuts(1, 5, 0.058, 1, 0., 32000., EFETcut,0,  0.055, 1, 0., 22000.,EFETcut,0 , EFMass );
	break;
      case TIGHT: // 70%
	//l2CalSel.setCuts(0, 0, 0, 0.,0.002735 , 0, 0, 63585);
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,L2IDETCut*4./3., 1);
	efSel.setCuts(1, 4, 0.043, 1, 0., 41000., EFETcut,0,  0.050, 1, 0., 36000., EFETcut,0 , EFMass );
	break;
      default :
	cout << "FIXME TrigTauSelectionManager has internal problem with menu " << TrigTauFlags::menuName(type) 
	     << " and level " << level << endl;break;
      }
      break;
   default :
     cout << ":setTauMenu MENU1520test can not deal with menu " << TrigTauFlags::menuName(type)  << endl;
     break;
   }


}

//=============================================================================================
void TrigTauSelectionManager::setTauMenu1560(TrigTauFlags::TRIGGERMENU type, CUTLEVEL level) {
  float EFETcut,  L2IDETCut , EFMass;
   switch (type) {
  case TrigTauFlags::TAUNOCUT: // Not really "no" cuts
    l1Sel.setCuts(0, 5000, 0, 0, 0, 0);
    l2CalSel.setCuts(0, 0, 0, 0.0, 0.0, 0.0, 0);
    l2TrkSel.setCuts(-1, -1, -1, -1, 7500, 0.1, 1500, 0.15, 0.30);
    efSel.setCuts(0, 100, 100., 10., -10., 0.);
    break;

  case TrigTauFlags::TAU12:
      l1Sel.setCuts(0, 5000, 0, 0, 0, 0);
      l2TrkSel.setCoreFromLeading(true);
       EFETcut = 12000;
       L2IDETCut = 15000;
       EFMass = 1600;
      //l2CalSel.setCuts(0, 0, 0, 0., 0.022085, 0., 0,5740 );
      l2CalSel.setCuts(0, 0, 0, 1.,0.0163 , 1., 0, 7550);
      switch (level ){
      case SOFT : //95%
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,L2IDETCut, 0);
	efSel.setCuts(1, 5, 0.180, 0, 0., 2000., EFETcut,0,  0.150, 0, 0., 3000., EFETcut,0, 0. );
	break;
      case LOOSE :     //90%
	//l2CalSel.setCuts(0, 0, 0, 0.,0.016325 , 0., 0, 7540);
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,L2IDETCut, 1);
	efSel.setCuts(1, 5, 0.105, 1, 0., 2500., EFETcut,0,  0.125, 1, 0., 3800., EFETcut,0, 0. );
	break;
      case MEDIUM:     //80%
	//l2CalSel.setCuts(0, 0, 0, 0., 0.01353 , 0 , 0, 8380);
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,L2IDETCut*4./3., 1);
	efSel.setCuts(1, 5, 0.075, 0, 0., 2000., EFETcut,0,  0.11, 0, 0., 5000.,EFETcut,0, EFMass );
	break;
      case TIGHT: // 70%
	//l2CalSel.setCuts(0, 0, 0, 0.,0.011555 , 0, 0, 9100);
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,L2IDETCut*4./3., 1);
	efSel.setCuts(1, 4, 0.074, 0, 0., 4000., EFETcut,0,  0.11, 0, 0., 7000., EFETcut,0, EFMass );
	break;
      default :
	cout << "FIXME TrigTauSelectionManager has internal problem with menu " << TrigTauFlags::menuName(type) 
	     << " and level " << level << endl;break;
      }
      break;


  case TrigTauFlags::TRK9:
      l1Sel.setCuts(0, 5000, 0, 0, 0, 0);
      l2TrkSel.setCoreFromLeading(true);
      //l2CalSel.setCuts(0, 0, 0, 0., 0.020105, 0., 0, 8585);
      l2CalSel.setCuts(0, 0, 0, 0., 0., 0., 0, 0.);
       EFMass = 1600;
      switch (level ){
      case SOFT :     //90%
      case LOOSE :     //90%
      case MEDIUM :     //90%
      case TIGHT :     //90%
	//l2CalSel.setCuts(0, 0, 0, 0., 0.014705, 0., 0, 10405);
	l2TrkSel.setCuts(1, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 9000, false, 0.1,0, 1);
	efSel.setCuts(1, 1, 100., 100., -10., 9000., -10.,0,  100, 100, 0., 9000., 0.,0, 0. );
	break;
      default :
	cout << "FIXME TrigTauSelectionManager has internal problem with menu " << TrigTauFlags::menuName(type) 
	     << " and level " << level << endl; break;
      }

    break;
    
  case TrigTauFlags::TAU16:
      l1Sel.setCuts(0, 6000, 0, 0, 0, 0);
      l2TrkSel.setCoreFromLeading(true);
      //l2CalSel.setCuts(0, 0, 0, 0., 0.020105, 0., 0, 8585);
      l2CalSel.setCuts(0, 0, 0, 1., 0.01470, 1., 0, 10400);
       EFMass = 1600;
      switch (level ){
      case LOOSE :     //90%
	//l2CalSel.setCuts(0, 0, 0, 0., 0.014705, 0., 0, 10405);
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,20000, 1);
	efSel.setCuts(1, 5, 0.1, 1, 0., 2500., 16000.,0,  0.12, 1, 0., 2500., 16000.,0, 0. );
	break;
      default :
	cout << "FIXME TrigTauSelectionManager has internal problem with menu " << TrigTauFlags::menuName(type) 
	     << " and level " << level << endl; break;
      }

    break;
    
//   case TrigTauFlags::TAU16I:
//       l1Sel.setCuts(0, 6000, 5000, 0, 0, 0);
//       l2TrkSel.setCoreFromLeading(true);
//       //l2CalSel.setCuts(0, 0, 0, 0., 0.020105, 0., 0, 8585);
//       l2CalSel.setCuts(0, 0, 0, 1., 0.01470, 1., 0, 10400);
//        EFMass = 1600;
//       switch (level ){
//       case SOFT : //95%
// 	//l2CalSel.setCuts(0, 0, 0, 0., 0.020105, 0., 0, 8585);
// 	l2TrkSel.setCuts(7, -1, -1, 7, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,20000., 0);
// 	efSel.setCuts(1, 5, 0.180, 0, 0., 2500., 16000.,0,  0.150, 0, 0., 4000., 16000.,0, 0. );
// 	break;
//       case LOOSE :     //90%
// 	//l2CalSel.setCuts(0, 0, 0, 0., 0.014705, 0., 0, 10405);
// 	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,20000, 1);
// 	efSel.setCuts(1, 5, 0.1, 1, 0., 2500., 16000.,0,  0.12, 1, 0., 2500., 16000.,0, 0. );
// 	break;
//       case MEDIUM:     //85%
// 	//l2CalSel.setCuts(0, 0, 0, 0., 0.012185, 0 , 0,11575 );
// 	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,26667., 1);
// 	efSel.setCuts(1, 5, 0.078, 1, 0., 2500., 16000.,0,  0.11, 1, 0., 7000., 16000.,0, EFMass );
// 	break;
//       case TIGHT: // 80%
// 	//l2CalSel.setCuts(0, 0, 0, 0.,0.010565 , 0, 0, 12355);
// 	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,26667., 1);
// 	efSel.setCuts(1, 4, 0.074, 1, 0., 4500., 16000.,0,  0.10, 1, 0., 9000., 16000.,0, EFMass );
// 	break;
//       default :
// 	cout << "FIXME TrigTauSelectionManager has internal problem with menu " << TrigTauFlags::menuName(type) 
// 	     << " and level " << level << endl; break;
//       }

//     break;
    
//   case TrigTauFlags::TAU20I:
//       l1Sel.setCuts(0, 6000, 5000, 0, 0, 0);
//       l2TrkSel.setCuts(7, -1, 2, 7, 7500, 0.15, 1500, 0.15, 0.30, 5000, true, 0.1, 0., 1);
//       l2TrkSel.setCoreFromLeading(true);
//       efSel.setCuts(1, 8, 0.1, 0.3, 0., 7000., 22000.);
//       l2CalSel.setCuts(0, 0, 0, 1., 0.0136, 1., 0, 13000);
//        EFMass = 1600;
//       switch (level ){
//       case SOFT : //95%
// 	//l2CalSel.setCuts(0, 0, 0, 0., 0.018575, 0., 0, 10750);
// 	l2TrkSel.setCuts(7, -1, -1, 7, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,25000., 0);
// 	efSel.setCuts(1, 5, 0.180, 0, 0., 2500., 20000.,0,  0.150, 0, 0., 4000., 20000.,0, 0.  );
// 	break;
//       case LOOSE :     //90%
// 	//l2CalSel.setCuts(0, 0, 0, 0., 0.013625, 0., 0, 12990);
// 	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,25000, 1);
// 	efSel.setCuts(1, 5, 0.100, 1, 0., 2500., 20000.,0,  0.12, 1, 0., 4500., 20000.,0 , 0. );
// 	break;
//       case MEDIUM:     //80%
// 	//l2CalSel.setCuts(0, 0, 0, 0., 0.011375 , 0 , 0, 14250);
// 	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,33000., 1);
// 	efSel.setCuts(1, 5, 0.078, 1, 0., 3500., 20000.,0,  0.11, 1, 0., 8000., 20000.,0, EFMass  );
// 	break;
//       case TIGHT: // 70%
// 	//l2CalSel.setCuts(0, 0, 0, 0.,0.009845 , 0, 0, 15090);
// 	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,33000., 1);
// 	efSel.setCuts(1, 4, 0.073, 1, 0., 5500., 20000.,0,  0.1, 1, 0., 10000., 20000.,0, EFMass  );
// 	break;
//       default :
// 	cout << "FIXME TrigTauSelectionManager has internal problem with menu " << TrigTauFlags::menuName(type) 
// 	     << " and level " << level << endl;break;
//       }
//     break;

  case TrigTauFlags::TAU29:
//   case TrigTauFlags::TAU29I:
//       if( type ==  TrigTauFlags::TAU29I)   l1Sel.setCuts(0, 11000, 5000, 0, 0, 0);
//       else                                 
      l1Sel.setCuts(0, 11000, 0, 0, 0, 0);
      
      //l2CalSel.setCuts(0, 26500, 0, 0.048, 0.015, 0.60, 0);
      //l2TrkSel.setCuts(7, -1, 2, 7, 7500, 0.15, 1500, 0.15, 0.30, 5000, true, 0.1, 0., 1);
      l2TrkSel.setCoreFromLeading(true);
      //efSel.setCuts(1, 8, 0.09, 0.24, 0., 8000., 31000.);
      //l2CalSel.setCuts(0, 0, 0, 0., 0.015155, 0., 0, 16040);
      l2CalSel.setCuts(0, 0, 0, 0., 0.0115, 0., 0, 18700);
       EFMass = 1600;
       EFETcut = 29000;
      switch (level ){
      case SOFT : //95%
	//l2CalSel.setCuts(0, 0, 0, 0., 0.015155, 0., 0, 16040);
	l2TrkSel.setCuts(7, -1, -1, 7, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,35000., 0);
	efSel.setCuts(1, 5, 0.180, 0, 0., 2500., EFETcut,0,  0.150, 0, 0., 4000., EFETcut,0 , 0. );
	break;
      case LOOSE :     //90%
	//l2CalSel.setCuts(0, 0, 0, 0., 0.011465, 0., 0, 18760);
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,35000, 1);
	efSel.setCuts(1, 5, 0.100, 1, 0., 3000., EFETcut,0,  0.12, 1, 0., 5000., EFETcut,0, 0.  );
	break;
      case MEDIUM:     //80%
	//l2CalSel.setCuts(0, 0, 0, 0., 0.009575 , 0 , 0,20360 );
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,46700., 1);
	efSel.setCuts(1, 5, 0.078, 1, 0., 5000., EFETcut,0,  0.11, 1, 0., 10000., EFETcut,0 , EFMass );
	break;
      case TIGHT: // 70%
	//l2CalSel.setCuts(0, 0, 0, 0.,0.008225 , 0, 0, 21800);
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,46700., 1);
	efSel.setCuts(1, 4, 0.073, 1, 0., 8000., EFETcut,0,  0.1, 1, 0., 13000., EFETcut,0 , EFMass );
	break;
      default :
	cout << "FIXME TrigTauSelectionManager has internal problem with menu " << TrigTauFlags::menuName(type) 
	     << " and level " << level << endl;break;
      }
    break;

  case TrigTauFlags::TAU38:
      l1Sel.setCuts(0, 11000, 0, 0, 0, 0);
      l2TrkSel.setCoreFromLeading(true);
       EFETcut = 38000;
       L2IDETCut = 45000;
      //l2CalSel.setCuts(0, 0, 0, 0., 0.012185, 0., 0, 22630);
      l2CalSel.setCuts(0, 0, 0, 0.,0.009 , 0., 0, 25800);
       EFMass = 1600;
      switch (level ){
      case SOFT : //95%
	//l2CalSel.setCuts(0, 0, 0, 0., 0.012185, 0., 0, 22630);
	l2TrkSel.setCuts(7, -1, -1, 7, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,L2IDETCut, 0);
	efSel.setCuts(1, 5, 0.180, 0, 0., 2500., EFETcut,0,  0.150, 0, 0., 4000., EFETcut,0, 0.  );
	break;
      case LOOSE :     //90%
	//l2CalSel.setCuts(0, 0, 0, 0.,0.008945 , 0., 0, 25870);
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,L2IDETCut, 1);
	efSel.setCuts(1, 5, 0.100, 1, 0., 3800., EFETcut,0,  0.120, 1, 0., 5500., EFETcut,0, 0.  );
	break;
      case MEDIUM:     //80%
	//l2CalSel.setCuts(0, 0, 0, 0., 0.007415 , 0 , 0, 27490);
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,L2IDETCut*4./3., 1);
	efSel.setCuts(1, 5, 0.078, 1, 0., 7000., EFETcut,0,  0.11, 1, 0., 12000.,EFETcut,0 , EFMass );
	break;
      case TIGHT: // 70%
	//l2CalSel.setCuts(0, 0, 0, 0.,0.006335 , 0, 0,29110 );
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,L2IDETCut*4./3., 1);
	efSel.setCuts(1, 4, 0.072, 1, 0., 11000., EFETcut,0,  0.090, 1, 0., 15000., EFETcut,0, EFMass  );
	break;
      default :
	cout << "FIXME TrigTauSelectionManager has internal problem with menu " << TrigTauFlags::menuName(type) 
	     << " and level " << level << endl;break;
      }
    break;

//   case TrigTauFlags::TAU38I:
//       l1Sel.setCuts(0, 11000, 5000, 0, 0, 0);
//       l2TrkSel.setCoreFromLeading(true);
//        EFETcut = 38000;
//       l2CalSel.setCuts(0, 0, 0, 0.,0.009 , 0., 0, 25800);
//        L2IDETCut = 45000;
//        EFMass = 1600;
//       switch (level ){
//       case SOFT : //95%
// 	//l2CalSel.setCuts(0, 0, 0, 0., 0.012185, 0., 0, 22630);
// 	l2TrkSel.setCuts(7, -1, -1, 7, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,L2IDETCut, 0);
// 	efSel.setCuts(1, 5, 0.180, 0, 0., 2500., EFETcut,0,  0.150, 0, 0., 4000., EFETcut,0, 0  );
// 	break;
//       case LOOSE :     //90%
// 	//l2CalSel.setCuts(0, 0, 0, 0.,0.008945 , 0., 0, 25870);
// 	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,L2IDETCut, 1);
// 	efSel.setCuts(1, 5, 0.100, 1, 0., 3800., EFETcut,0,  0.120, 1, 0., 5500., EFETcut,0, 0.  );
// 	break;
//       case MEDIUM:     //80%
// 	//l2CalSel.setCuts(0, 0, 0, 0., 0.007415 , 0 , 0, 27490);
// 	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,L2IDETCut*4./3., 1);
// 	efSel.setCuts(1, 5, 0.078, 0, 0., 7000., EFETcut,0,  0.11, 0, 0., 12000.,EFETcut,0 , EFMass );
// 	break;
//       case TIGHT: // 70%
// 	//l2CalSel.setCuts(0, 0, 0, 0.,0.006335 , 0, 0,29110 );
// 	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,L2IDETCut*4./3., 1);
// 	efSel.setCuts(1, 4, 0.072, 0, 0., 11000., EFETcut,0,  0.090, 0, 0., 15000., EFETcut,0, EFMass  );
// 	break;      
//       default :
// 	cout << "FIXME TrigTauSelectionManager has internal problem with menu " << TrigTauFlags::menuName(type) 
// 	     << " and level " << level << endl;break;
//       }
//     break;

  case TrigTauFlags::TAU50:
      l1Sel.setCuts(0, 20000, 0, 0, 0, 0);
      l2TrkSel.setCoreFromLeading(true);
      //l2CalSel.setCuts(0, 0, 0, 0., 0.009125, 0., 0, 31645);
      l2CalSel.setCuts(0, 0, 0, 0., 0.0049, 0., 0, 36000);
       EFMass = 1600;
       L2IDETCut = 60000;
       EFETcut= 50000;
      switch (level ){
      case SOFT : //95%
	//l2CalSel.setCuts(0, 0, 0, 0., 0.012185, 0., 0, 22630);
	l2TrkSel.setCuts(7, -1, -1, 7, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,L2IDETCut, 0);
	efSel.setCuts(1, 5, 0.180, 0, 0., 6000., EFETcut,0,  0.180, 0, 0., 6000., EFETcut,0, 0.  );
	break;
      case LOOSE :     //90%
	//l2CalSel.setCuts(0, 0, 0, 0., 0.004895, 0., 0, 36055);
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,L2IDETCut, 1);
	efSel.setCuts(1, 5, 0.090, 1, 0., 10000., EFETcut,0,  0.090, 1, 0., 10000., EFETcut,0, 0.  );
	break;
      case MEDIUM:     //80%
	//l2CalSel.setCuts(0, 0, 0, 0., 0.003725 , 0 , 0, 38995);
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,L2IDETCut*4./3., 1);
	efSel.setCuts(1, 5, 0.060, 1, 0., 17000., EFETcut,0,  0.068, 1, 0., 15000.,EFETcut,0, EFMass  );
	break;
      case TIGHT: // 70%
	///l2CalSel.setCuts(0, 0, 0, 0.,0.003095 , 0, 0, 41095);
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,L2IDETCut*4./3., 1);
	efSel.setCuts(1, 4, 0.050, 1, 0., 21000., EFETcut,0,  0.065, 1, 0., 25000., EFETcut,0 , EFMass );
	break;
      default :
	cout << "FIXME TrigTauSelectionManager has internal problem with menu " << TrigTauFlags::menuName(type) 
	     << " and level " << level << endl;break;
      }
    break;

  case TrigTauFlags::TAU84:
      l1Sel.setCuts(0, 20000, 0, 0, 0, 0);
      l2CalSel.setCuts(0, 75000, 0, 0.04, 0.00345, 0.35, 0);
      l2TrkSel.setCuts(7, -1, 2, 7, 7500, 0.15, 1500, 0.15, 0.30, 5000, true, 0.2, 0., 1);
      l2TrkSel.setCoreFromLeading(true);
      efSel.setCuts(1, 8, 0.09, 0.24, 0., 8000., 90000.);
       EFETcut = 84000;
       L2IDETCut = 100000;
      //l2CalSel.setCuts(0, 0, 0, 0., 0.007685, 0., 0, 47345);
      l2CalSel.setCuts(0, 0, 0, 1.,0.0042 , 1., 0,56000 );
       EFMass=1600;
      switch (level ){
      case SOFT : //95%
	//l2CalSel.setCuts(0, 0, 0, 0., 0.007685, 0., 0, 47345);
	l2TrkSel.setCuts(7, -1, -1, 7, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,L2IDETCut, 0);
	efSel.setCuts(1, 5, 0.180, 0, 0., 7000., EFETcut,0,  0.140, 0, 0., 9000., EFETcut,0, 0.  );
	break;
      case LOOSE :     //90%
	//l2CalSel.setCuts(0, 0, 0, 0.,0.004175 , 0., 0,56045 );
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,L2IDETCut, 1);
	efSel.setCuts(1, 5, 0.080, 1, 0., 12000., EFETcut,0,  0.085, 1, 0., 13000., EFETcut,0 , 0. );
	break;
      case MEDIUM:     //80%
	//l2CalSel.setCuts(0, 0, 0, 0., 0.003275 , 0 , 0,60105 );
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,L2IDETCut*4./3., 1);
	efSel.setCuts(1, 5, 0.060, 1, 0., 25000., EFETcut,0,  0.068, 1, 0., 25000.,EFETcut,0 , EFMass );
	break;
      case TIGHT: // 70%
	//l2CalSel.setCuts(0, 0, 0, 0.,0.002735 , 0, 0, 63585);
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,L2IDETCut*4./3., 1);
	efSel.setCuts(1, 4, 0.046, 1, 0., 30000., EFETcut,0,  0.055, 1, 0., 31000., EFETcut,0 , EFMass );
	break;
      default :
	cout << "FIXME TrigTauSelectionManager has internal problem with menu " << TrigTauFlags::menuName(type) 
	     << " and level " << level << endl;break;
      }
    break;

  case TrigTauFlags::TAU125:
      l1Sel.setCuts(0, 20000, 0, 0, 0, 0);
      l2TrkSel.setCoreFromLeading(true);
       EFETcut =125000;
       L2IDETCut = 150000;
      l2CalSel.setCuts(0, 0, 0, 1.,0.0034 , 1., 0, 93000.);
       EFMass=1600;
      switch (level ){
      case SOFT : //95%
	//l2CalSel.setCuts(0, 0, 0, 0., 0.007685, 0., 0, 47345);
	l2TrkSel.setCuts(7, -1, -1, 7, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,L2IDETCut, 0);
	efSel.setCuts(1, 5, 0.180, 0, 0., 7000., EFETcut,0,  0.140, 0, 0., 9000., EFETcut,0, 0.  );
	break;
      case LOOSE :     //90%
	//l2CalSel.setCuts(0, 0, 0, 0.,0.004175 , 0., 0,56045 );
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,L2IDETCut, 1);
	efSel.setCuts(1, 5, 0.07, 1, 0., 15000., EFETcut,0,  0.08, 1, 0., 16000., EFETcut,0 , 0. );
	break;
      case MEDIUM:     //80%
	//l2CalSel.setCuts(0, 0, 0, 0., 0.003275 , 0 , 0,60105 );
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,L2IDETCut*4./3., 1);
	efSel.setCuts(1, 5, 0.058, 1, 0., 32000., EFETcut,0,  0.055, 1, 0., 22000.,EFETcut,0 , EFMass );
	break;
      case TIGHT: // 70%
	//l2CalSel.setCuts(0, 0, 0, 0.,0.002735 , 0, 0, 63585);
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,L2IDETCut*4./3., 1);
	efSel.setCuts(1, 4, 0.043, 1, 0., 41000., EFETcut,0,  0.050, 1, 0., 36000., EFETcut,0 , EFMass );
	break;
      default :
	cout << "FIXME TrigTauSelectionManager has internal problem with menu " << TrigTauFlags::menuName(type) 
	     << " and level " << level << endl;break;
      }
      break;
   default :
     cout << ":setTauMenu MENU1560 can not deal with menu " << TrigTauFlags::menuName(type)  << endl;
     break;
   }


}

//=============================================================================================
void TrigTauSelectionManager::setTauMenu1600(TrigTauFlags::TRIGGERMENU type, CUTLEVEL level) {
  float EFETcut,  L2IDETCut , EFMass;
   switch (type) {
  case TrigTauFlags::TAUNOCUT: // Not really "no" cuts
    l1Sel.setCuts(0, 5000, 0, 0, 0, 0);
    l2CalSel.setCuts(0, 0, 0, 0.0, 0.0, 0.0, 0);
    l2TrkSel.setCuts(15, -1, -1, -1, 7500, 0.1, 1500, 0.15, 0.30, 0, false);
    efSel.setCuts(0, 100, 100., 10., -10., 0.);
    break;

  case TrigTauFlags::TAU12:
      l1Sel.setCuts(0, 5000, 0, 0, 0, 0);
      l2TrkSel.setCoreFromLeading(true);
       EFETcut = 12000;
       L2IDETCut = 0;
       EFMass = 1600;
       //l2CalSel.setCuts(0, 0, 0, 0., 0.022085, 0., 0,5740 );
       l2CalSel.setCuts(0, 0, 0, 999.,0.0178 , 999., 0, 7550);
      switch (level ){
      case SOFT : //95%
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,L2IDETCut, 0);
	efSel.setCuts(1, 5, 0.180, 0, 0., 2000., EFETcut,0,  0.150, 0, 0., 3000., EFETcut,0, 0. );
	break;
      case LOOSE :     //90%
	//l2CalSel.setCuts(0, 0, 0, 0.,0.016325 , 0., 0, 7540);
	l2TrkSel.setCuts(10, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,L2IDETCut, 1);
	efSel.setCuts(1, 10, 0.105, 999, 0., 2500., EFETcut,0,  0.125, 1, 0., 2500., EFETcut,0, 0. );
	break;
      case MEDIUM:     //80%
        l2CalSel.setCuts(0, 0, 0, 999.,0.0124 , 999., 0, 7000);
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 0, 0.15, 0.30, 0, false, 0.1, 0, 1);
	efSel.setCuts( 1, 6, 0.082, 0, 0., 0., EFETcut, 0,  0.098, 0, 0., 0., -1, -1, 0, 0.086, 0.076 );
	break;
      case TIGHT: // 70%
	//l2CalSel.setCuts(0, 0, 0, 0.,0.011555 , 0, 0, 9100);
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,L2IDETCut*4./3., 1);
	efSel.setCuts(1, 4, 0.074, 0, 0., 4000., EFETcut,0,  0.11, 0, 0., 7000., EFETcut,0, EFMass );
	break;
      default :
	cout << "FIXME TrigTauSelectionManager has internal problem with menu " << TrigTauFlags::menuName(type) 
	     << " and level " << level << endl;break;
      }
      break;


  case TrigTauFlags::TRK9:
      l1Sel.setCuts(0, 5000, 0, 0, 0, 0);
      l2TrkSel.setCoreFromLeading(true);
      //l2CalSel.setCuts(0, 0, 0, 0., 0.020105, 0., 0, 8585);
      l2CalSel.setCuts(0, 0, 0, 0., 0., 0., 0, 0.);
       EFMass = 1600;
      switch (level ){
      case SOFT :     //90%
      case LOOSE :     //90%
      case MEDIUM :     //90%
      case TIGHT :     //90%
	//l2CalSel.setCuts(0, 0, 0, 0., 0.014705, 0., 0, 10405);
	l2TrkSel.setCuts(1, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 9000, false, 0.1,0, 1);
	efSel.setCuts(1, 1, 100., 100., -10., 9000., -10.,0,  100, 100, 0., 9000., 0.,0, 0. );
	break;
      default :
	cout << "FIXME TrigTauSelectionManager has internal problem with menu " << TrigTauFlags::menuName(type) 
	     << " and level " << level << endl; break;
      }

    break;
    
  case TrigTauFlags::TAU16:
//   case TrigTauFlags::TAU16I:
//       if( type ==  TrigTauFlags::TAU16I)   l1Sel.setCuts(0, 6000, 5000, 0, 0, 0);
//       else                                 
      l1Sel.setCuts(0, 6000, 0, 0, 0, 0);
      l2TrkSel.setCoreFromLeading(true);
      l2CalSel.setCuts(0, 0, 0, 999., 0.0162, 999., 0, 9000);
       EFMass = 1600;
      switch (level ){
      case LOOSE :     //90%
	//l2CalSel.setCuts(0, 0, 0, 0., 0.014705, 0., 0, 10405);
	l2TrkSel.setCuts(10, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1, 0, 1);
	efSel.setCuts(1, 10, 0.1, 999, 0., 2500., 16000.,0,  0.12, 1, 0., 2500., 16000.,0, 0. );
	break;
      case MEDIUM:     //85%
        l2CalSel.setCuts(0, 0, 0, 999., 0.0124, 999., 0, 9000);
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1, 0., 1);
	efSel.setCuts(1, 6, 0.080, 999., 0., 0., 16000.,0, 0.092, 1, 0., 0., 16000.,0, 0., 0.076, 0.062);
	break;
      case TIGHT: // 80%
	//l2CalSel.setCuts(0, 0, 0, 0.,0.010565 , 0, 0, 12355);
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,26667., 1);
	efSel.setCuts(1, 4, 0.074, 1, 0., 4500., 16000.,0,  0.10, 1, 0., 9000., 16000.,0, EFMass );
	break;
      default :
	cout << "FIXME TrigTauSelectionManager has internal problem with menu " << TrigTauFlags::menuName(type) 
	     << " and level " << level << endl; break;
      }

    break;
    
  case TrigTauFlags::TAU20:
//   case TrigTauFlags::TAU20I:
//       if( type ==  TrigTauFlags::TAU20I)   l1Sel.setCuts(0, 6000, 5000, 0, 0, 0);
//       else                                 
      l1Sel.setCuts(0, 6000, 0, 0, 0, 0);
      
      l2TrkSel.setCoreFromLeading(true);
      l2CalSel.setCuts(0, 0, 0, 999., 0.0151, 999., 0, 11500);
      EFMass = 1600;
      switch (level ){
      case LOOSE :     //90%
	l2TrkSel.setCuts(10, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1, 0, 1);
	efSel.setCuts(1, 10, 0.100, 999, 0., 2500., 20000.,0,  0.12, 1, 0., 4500., 20000.,0 , 0. );
	break;
      case MEDIUM:     //80%
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,33000., 1);
	efSel.setCuts(1, 5, 0.078, 1, 0., 3500., 20000.,0,  0.11, 1, 0., 8000., 20000.,0, EFMass  );
	break;
      case TIGHT: // 70%
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,33000., 1);
	efSel.setCuts(1, 4, 0.073, 1, 0., 5500., 20000.,0,  0.1, 1, 0., 10000., 20000.,0, EFMass  );
	break;
      default :
	cout << "FIXME TrigTauSelectionManager has internal problem with menu " << TrigTauFlags::menuName(type) 
	     << " and level " << level << endl;break;
      }
    break;

  case TrigTauFlags::TAU29:
//   case TrigTauFlags::TAU29I:
  case TrigTauFlags::TAU29v2:
//       if( type ==  TrigTauFlags::TAU29I)   l1Sel.setCuts(0, 11000, 5000, 0, 0, 0);
//       else                                 
      l1Sel.setCuts(0, 11000, 0, 0, 0, 0);
      
      //l2CalSel.setCuts(0, 26500, 0, 0.048, 0.015, 0.60, 0);
      //l2TrkSel.setCuts(7, -1, 2, 7, 7500, 0.15, 1500, 0.15, 0.30, 5000, true, 0.1, 0., 1);
      l2TrkSel.setCoreFromLeading(true);
      //efSel.setCuts(1, 8, 0.09, 0.24, 0., 8000., 31000.);
      //l2CalSel.setCuts(0, 0, 0, 0., 0.015155, 0., 0, 16040);
      l2CalSel.setCuts(0, 0, 0, 999, 0.0130, 999, 0, 17500);
      EFMass = 1600;
      EFETcut = 29000;
      switch (level ){
      case LOOSE :     //90%
	//l2CalSel.setCuts(0, 0, 0, 0., 0.011465, 0., 0, 18760);
	if( type ==  TrigTauFlags::TAU29v2)
        {
          l2TrkSel.setCuts(4, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1, 0, 1);
          efSel.setCuts(1, 3, 0.100, 999, 0., 3000., EFETcut,0,  0.120, 1, 0., 5000., EFETcut,0, 0., 100, 100);
        }
        else
        {
          l2TrkSel.setCuts(10, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1, 0, 1);
          efSel.setCuts(1, 10, 0.100, 999, 0., 3000., EFETcut,0,  0.120, 1, 0., 5000., EFETcut,0, 0., 100, 100);
        }
        break;
      case MEDIUM:     //80%
	//l2CalSel.setCuts(0, 0, 0, 0., 0.009575 , 0 , 0,20360 );
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,46700., 1);
	efSel.setCuts(1, 5, 0.078, 1, 0., 5000., EFETcut,0,  0.11, 1, 0., 10000., EFETcut,0 , EFMass );
	break;
      case TIGHT: // 70%
	//l2CalSel.setCuts(0, 0, 0, 0.,0.008225 , 0, 0, 21800);
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,46700., 1);
	efSel.setCuts(1, 4, 0.073, 1, 0., 8000., EFETcut,0,  0.1, 1, 0., 13000., EFETcut,0 , EFMass );
	break;
      default :
	cout << "FIXME TrigTauSelectionManager has internal problem with menu " << TrigTauFlags::menuName(type) 
	     << " and level " << level << endl;break;
      }
    break;

  case TrigTauFlags::TAU38:
      l1Sel.setCuts(0, 11000, 0, 0, 0, 0);
      l2TrkSel.setCoreFromLeading(true);
       EFETcut = 38000;
       L2IDETCut = 0;
      //l2CalSel.setCuts(0, 0, 0, 0., 0.012185, 0., 0, 22630);
      l2CalSel.setCuts(0, 0, 0, 999.,0.0117 , 999., 0, 19000);
       EFMass = 1600;
      switch (level ){
      case SOFT : //95%
	//l2CalSel.setCuts(0, 0, 0, 0., 0.012185, 0., 0, 22630);
	l2TrkSel.setCuts(7, -1, -1, 7, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,L2IDETCut, 0);
	efSel.setCuts(1, 5, 0.180, 0, 0., 2500., EFETcut,0,  0.150, 0, 0., 4000., EFETcut,0, 0.  );
	break;
      case LOOSE :     //90%
	//l2CalSel.setCuts(0, 0, 0, 0.,0.008945 , 0., 0, 25870);
	l2TrkSel.setCuts(10, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,L2IDETCut, 1);
	efSel.setCuts(1, 20, 0.072, 1, 0., 0., EFETcut,0,  0.088, 1, 0., 0., EFETcut,0, 0., 0.060, 0.048);
	break;
      case MEDIUM:     //80%
        l2CalSel.setCuts(0, 0, 0, 1.,0.009, 1., 0, 25800);
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05, 60000., 1);
	efSel.setCuts(1, 5, 0.078, 1, 0., 7000., EFETcut,0,  0.11, 1, 0., 12000., EFETcut, 0, EFMass, 100, 100);
	break;
      case TIGHT: // 70%
	//l2CalSel.setCuts(0, 0, 0, 0.,0.006335 , 0, 0,29110 );
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,L2IDETCut*4./3., 1);
	efSel.setCuts(1, 4, 0.072, 1, 0., 11000., EFETcut,0,  0.090, 1, 0., 15000., EFETcut,0, EFMass  );
	break;
      default :
	cout << "FIXME TrigTauSelectionManager has internal problem with menu " << TrigTauFlags::menuName(type) 
	     << " and level " << level << endl;break;
      }
    break;

//   case TrigTauFlags::TAU38I:
//       l1Sel.setCuts(0, 11000, 5000, 0, 0, 0);
//       l2TrkSel.setCoreFromLeading(true);
//        EFETcut = 38000;
//       l2CalSel.setCuts(0, 0, 0, 0.,0.009 , 0., 0, 25800);
//        L2IDETCut = 45000;
//        EFMass = 1600;
//       switch (level ){
//       case SOFT : //95%
// 	//l2CalSel.setCuts(0, 0, 0, 0., 0.012185, 0., 0, 22630);
// 	l2TrkSel.setCuts(7, -1, -1, 7, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,L2IDETCut, 0);
// 	efSel.setCuts(1, 5, 0.180, 0, 0., 2500., EFETcut,0,  0.150, 0, 0., 4000., EFETcut,0, 0  );
// 	break;
//       case LOOSE :     //90%
// 	//l2CalSel.setCuts(0, 0, 0, 0.,0.008945 , 0., 0, 25870);
// 	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,L2IDETCut, 1);
// 	efSel.setCuts(1, 5, 0.100, 1, 0., 3800., EFETcut,0,  0.120, 1, 0., 5500., EFETcut,0, 0.  );
// 	break;
//       case MEDIUM:     //80%
// 	//l2CalSel.setCuts(0, 0, 0, 0., 0.007415 , 0 , 0, 27490);
// 	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,L2IDETCut*4./3., 1);
// 	efSel.setCuts(1, 5, 0.078, 0, 0., 7000., EFETcut,0,  0.11, 0, 0., 12000.,EFETcut,0 , EFMass );
// 	break;
//       case TIGHT: // 70%
// 	//l2CalSel.setCuts(0, 0, 0, 0.,0.006335 , 0, 0,29110 );
// 	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,L2IDETCut*4./3., 1);
// 	efSel.setCuts(1, 4, 0.072, 0, 0., 11000., EFETcut,0,  0.090, 0, 0., 15000., EFETcut,0, EFMass  );
// 	break;      
//       default :
// 	cout << "FIXME TrigTauSelectionManager has internal problem with menu " << TrigTauFlags::menuName(type) 
// 	     << " and level " << level << endl;break;
//       }
//     break;

  case TrigTauFlags::TAU50:
      l1Sel.setCuts(0, 20000, 0, 0, 0, 0);
      l2TrkSel.setCoreFromLeading(true);
      l2CalSel.setCuts(0, 0, 0, 999., 0.01, 999., 0, 26500);
       EFMass = 1600;
       L2IDETCut = 0;
       EFETcut= 50000;
      switch (level ){
      case SOFT : //95%
	//l2CalSel.setCuts(0, 0, 0, 0., 0.012185, 0., 0, 22630);
	l2TrkSel.setCuts(7, -1, -1, 7, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,L2IDETCut, 0);
	efSel.setCuts(1, 5, 0.180, 0, 0., 6000., EFETcut,0,  0.180, 0, 0., 6000., EFETcut,0, 0.  );
	break;
      case LOOSE :     //90%
	//l2CalSel.setCuts(0, 0, 0, 0., 0.004895, 0., 0, 36055);
	l2TrkSel.setCuts(10, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,L2IDETCut, 1);
	efSel.setCuts(1, 20, 0.062, 1, 0., 0., EFETcut, 0, 0.076, 1, 0., 0., EFETcut,0, 0., 0.044, 0.038);
	break;
      case MEDIUM:     //80%
        l2CalSel.setCuts(0, 0, 0, 1., 0.009, 1., 0, 35000);
	//l2CalSel.setCuts(0, 0, 0, 0., 0.003725 , 0 , 0, 38995);
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05, 0, 1);
	efSel.setCuts(1, 6, 0.050, 1, 0., 0., EFETcut,0,  0.070, 1, 0., 0.,EFETcut,0, 0, 0.03, 0.03);
        break;
      case TIGHT: // 70%
        l2CalSel.setCuts(0, 0, 0, 1., 0.0049, 1., 0, 36000);
	///l2CalSel.setCuts(0, 0, 0, 0.,0.003095 , 0, 0, 41095);
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,L2IDETCut*4./3., 1);
	efSel.setCuts(1, 4, 0.050, 1, 0., 21000., EFETcut,0,  0.065, 1, 0., 25000., EFETcut,0 , EFMass );
	break;
      default :
	cout << "FIXME TrigTauSelectionManager has internal problem with menu " << TrigTauFlags::menuName(type) 
	     << " and level " << level << endl;break;
      }
    break;

  case TrigTauFlags::TAU84:
      l1Sel.setCuts(0, 20000, 0, 0, 0, 0);
      l2TrkSel.setCoreFromLeading(true);
       EFETcut = 84000;
       L2IDETCut = 100000;
       l2CalSel.setCuts(0, 0, 0, 999.,0.0072 , 999., 0, 47900 );
       EFMass=1600;
      switch (level ){
      case SOFT : //95%
	//l2CalSel.setCuts(0, 0, 0, 0., 0.007685, 0., 0, 47345);
	l2TrkSel.setCuts(7, -1, -1, 7, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,L2IDETCut, 0);
	efSel.setCuts(1, 5, 0.180, 0, 0., 7000., EFETcut,0,  0.140, 0, 0., 9000., EFETcut,0, 0.  );
	break;
      case LOOSE :     //90%
	//l2CalSel.setCuts(0, 0, 0, 0.,0.004175 , 0., 0,56045 );
	l2TrkSel.setCuts(10, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1, 0, 1);
	efSel.setCuts(1, 20, 0.050, 1, 0., 0., EFETcut,0,  0.064, 1, 0., 0., EFETcut,0 , 0., 0.030, 0.032);
	break;
      case MEDIUM:     //80%
	//l2CalSel.setCuts(0, 0, 0, 0., 0.003275 , 0 , 0,60105 );
        l2CalSel.setCuts(0, 0, 0, 1., 0.0070, 1., 0, 60000 );
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.02, 0, 1);
	efSel.setCuts(1, 6, 0.045, 1, 0., 0., EFETcut,0,  0.060, 1, 0., 0.,EFETcut,0 , 0., 0.025, 0.027);
	break;
      case TIGHT: // 70%
	//l2CalSel.setCuts(0, 0, 0, 0.,0.002735 , 0, 0, 63585);
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,L2IDETCut*4./3., 1);
	efSel.setCuts(1, 4, 0.046, 1, 0., 30000., EFETcut,0,  0.055, 1, 0., 31000., EFETcut,0 , EFMass );
	break;
      default :
	cout << "FIXME TrigTauSelectionManager has internal problem with menu " << TrigTauFlags::menuName(type) 
	     << " and level " << level << endl;break;
      }
    break;

  case TrigTauFlags::TAU125:
      l1Sel.setCuts(0, 50000, 0, 0, 0, 0);
      l2TrkSel.setCoreFromLeading(true);
       EFETcut =125000;
       L2IDETCut = 150000;
      l2CalSel.setCuts(0, 0, 0, 1.,0.0034 , 1., 0, 93000.);
       EFMass=1600;
      switch (level ){
      case SOFT : //95%
	//l2CalSel.setCuts(0, 0, 0, 0., 0.007685, 0., 0, 47345);
	l2TrkSel.setCuts(7, -1, -1, 7, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,L2IDETCut, 0);
	efSel.setCuts(1, 5, 0.180, 0, 0., 7000., EFETcut,0,  0.140, 0, 0., 9000., EFETcut,0, 0.  );
	break;
      case LOOSE :     //90%
	//l2CalSel.setCuts(0, 0, 0, 0.,0.004175 , 0., 0,56045 );
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.1,L2IDETCut, 1);
	efSel.setCuts(1, 5, 0.07, 1, 0., 15000., EFETcut,0,  0.08, 1, 0., 16000., EFETcut,0 , 0. );
	break;
      case MEDIUM:     //80%
	//l2CalSel.setCuts(0, 0, 0, 0., 0.003275 , 0 , 0,60105 );
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,L2IDETCut*4./3., 1);
	efSel.setCuts(1, 5, 0.058, 1, 0., 32000., EFETcut,0,  0.055, 1, 0., 22000.,EFETcut,0 , EFMass );
	break;
      case TIGHT: // 70%
	//l2CalSel.setCuts(0, 0, 0, 0.,0.002735 , 0, 0, 63585);
	l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false, 0.05,L2IDETCut*4./3., 1);
	efSel.setCuts(1, 4, 0.043, 1, 0., 41000., EFETcut,0,  0.050, 1, 0., 36000., EFETcut,0 , EFMass );
	break;
      default :
	cout << "FIXME TrigTauSelectionManager has internal problem with menu " << TrigTauFlags::menuName(type) 
	     << " and level " << level << endl;break;
      }
      break;
   default :
     cout << ":setTauMenu MENU1600 can not deal with menu " << TrigTauFlags::menuName(type)  << endl;
     break;
   }


}

void
TrigTauSelectionManager::setParametrizedEFCuts( TrigTauEFSel& efSel,
                                                CUTLEVEL level, float thresh)
{
  int nTrackMaxLoose = 10;
  if( thresh > 49000)
      nTrackMaxLoose = 20;
  
  switch( level)
  {
  case( LOOSE):
      efSel.setParamSafeCuts(
          TrigTauEFSel::CutFunctor( 0.663,  0.063,  -2.04e-4), // LowEMRad_OP
          TrigTauEFSel::CutFunctor( 0.339,  0.179,  -5.17e-4), // LowEMRad_MP
          TrigTauEFSel::CutFunctor( 0.0328, 0.134,  -2.72e-4), // LowTrkAvgD_OP
          TrigTauEFSel::CutFunctor( 0.695,  0.0565, -1.44e-4), // LowTrkAvgD_MP
          999., 4.55, 0.055, 0.142,   // LowEtOPt_SP/_MP, EMRad_SP/_MP
          0.113, 0.0536, 999., 4.55,  // TrkAvgD_SP/_MP,  EtOPt_SP/_MP
          1, nTrackMaxLoose, thresh); // MinNTrack, MaxNtrack, MinEtCalib
      break;
  case( MEDIUM1):
      efSel.setParamSafeCuts(
          TrigTauEFSel::CutFunctor( 0.715,  0.0522, -1.71e-4), // LowEMRad_OP
          TrigTauEFSel::CutFunctor( 0.447,  0.162,  -4.62e-4), // LowEMRad_MP
          TrigTauEFSel::CutFunctor( 0.0328, 0.134,  -2.72e-4), // LowTrkAvgD_OP
          TrigTauEFSel::CutFunctor( 0.810,  0.04,   -9.90e-5), // LowTrkAvgD_MP
          8.33, 3.33, 0.0475, 0.13, // LowEtOPt_SP/_MP, EMRad_SP/_MP
          0.113, 0.04, 8.33, 3.33,  // TrkAvgD_SP/_MP, EtOPt_SP/_MP
          1, 3, thresh);            // MinNTrack, MaxNtrack, MinEtCalib
      break;
  case( MEDIUM):
      efSel.setParamSafeCuts(
          TrigTauEFSel::CutFunctor( 0.715,  0.0522, -1.71e-4), // LowEMRad_OP
          TrigTauEFSel::CutFunctor( 0.447,  0.162,  -4.62e-4), // LowEMRad_MP
          TrigTauEFSel::CutFunctor( 0.0328, 0.134,  -2.72e-4), // LowTrkAvgD_OP
          TrigTauEFSel::CutFunctor( 0.810,  0.04,   -9.90e-5), // LowTrkAvgD_MP
          8.33, 3.33, 0.0475, 0.13, // LowEtOPt_SP/_MP, EMRad_SP/_MP
          0.113, 0.04, 8.33, 3.33,  // TrkAvgD_SP/_MP, EtOPt_SP/_MP
          1, 6, thresh);            // MinNTrack, MaxNtrack, MinEtCalib
      break;
  case( TIGHT):
      efSel.setParamSafeCuts(
          TrigTauEFSel::CutFunctor( 0.819,  0.0306, -1.03e-4), // LowEMRad_OP
          TrigTauEFSel::CutFunctor( 0.930,  0.0833, -2.15e-4), // LowEMRad_MP
          TrigTauEFSel::CutFunctor( 0.0328, 0.134,  -2.72e-4), // LowTrkAvgD_OP
          TrigTauEFSel::CutFunctor( 0.879,  0.0146, -7.21e-5), // LowTrkAvgD_MP
          7.14, 2.5, 0.0325, 0.0777, // LowEtOPt_SP/_MP, EMRad_SP/_MP
          0.113, 0.0198, 7.14, 2.5,  // TrkAvgD_SP/_MP, EtOPt_SP/_MP
          1, 6, thresh);             // MinNTrack, MaxNtrack, MinEtCalib
  default:
    break;
  }
}

void
TrigTauSelectionManager::setParametrizedEFCuts_201104( TrigTauEFSel& efSel,
                                                       CUTLEVEL level, float thresh)
{
  int nTrackMaxLoose = 10;
  if( thresh > 49000)
      nTrackMaxLoose = 20;
  
  float c_0_EMRadius_1p_loose          = 0.365      ;//0.197      ;
  float c_1_EMRadius_1p_loose          = 0.190      ;//0.157      ;
  float c_2_EMRadius_1p_loose          = -0.00085   ;
  float c_3_EMRadius_1p_loose          = 0.18       ;//0.109      ;
  float c_0_trkAvgDist_1p_loose        = 0.827      ;//0.447      ;
  float c_1_trkAvgDist_1p_loose        = 0.140      ;//0.116      ;
  float c_2_trkAvgDist_1p_loose        = -0.000213  ;
  float c_3_trkAvgDist_1p_loose        = 0.151      ;//0.111      ;
  float EtOverPtLeadTrk_1p_loose       = 999.0      ;
  float c_0_EMRadius_1p_medium         = 0.516      ;
  float c_1_EMRadius_1p_medium         = 0.20       ;//0.0818     ;
  float c_2_EMRadius_1p_medium         = -0.000376  ;
  float c_3_EMRadius_1p_medium         = 0.095      ;//0.0678     ;
  float c_0_trkAvgDist_1p_medium       = 0.626      ;//0.417      ;
  float c_1_trkAvgDist_1p_medium       = 0.27       ;//0.0924     ;
  float c_2_trkAvgDist_1p_medium       = -0.000392  ;//-0.000261  ;
  float c_3_trkAvgDist_1p_medium       = 0.158      ;//0.0837     ;
  float EtOverPtLeadTrk_1p_medium      = 8.33       ;
  float c_0_EMRadius_1p_tight          = 0.616      ;
  float c_1_EMRadius_1p_tight          = 0.0584     ;
  float c_2_EMRadius_1p_tight          = -0.000227  ;
  float c_3_EMRadius_1p_tight          = 0.055      ;
  float c_0_trkAvgDist_1p_tight        = 0.381      ;
  float c_1_trkAvgDist_1p_tight        = 0.0643     ;
  float c_2_trkAvgDist_1p_tight        = -0.000318  ;
  float c_3_trkAvgDist_1p_tight        = 0.0515     ;
  float EtOverPtLeadTrk_1p_tight       = 6.25       ;
  float c_0_EMRadius_3p_loose          = -0.00194   ;
  float c_1_EMRadius_3p_loose          = 0.203      ;
  float c_2_EMRadius_3p_loose          = -0.00115   ;
  float c_3_EMRadius_3p_loose          = 0.134      ;
  float c_0_trkAvgDist_3p_loose        = 0.441      ;
  float c_1_trkAvgDist_3p_loose        = 0.111      ;
  float c_2_trkAvgDist_3p_loose        = -0.000222  ;
  float c_3_trkAvgDist_3p_loose        = 0.105      ;
  float EtOverPtLeadTrk_3p_loose       = 5          ;
  float c_0_EMRadius_3p_medium         = 0.197      ;
  float c_1_EMRadius_3p_medium         = 0.157      ;
  float c_2_EMRadius_3p_medium         = -0.00085   ;
  float c_3_EMRadius_3p_medium         = 0.109      ;
  float c_0_trkAvgDist_3p_medium       = 0.375      ;
  float c_1_trkAvgDist_3p_medium       = 0.0596     ;
  float c_2_trkAvgDist_3p_medium       = -0.000328  ;
  float c_3_trkAvgDist_3p_medium       = 0.0461     ;
  float EtOverPtLeadTrk_3p_medium      = 3.33       ;
  float c_0_EMRadius_3p_tight          = 0.436      ;
  float c_1_EMRadius_3p_tight          = 0.1        ;
  float c_2_EMRadius_3p_tight          = -0.000494  ;
  float c_3_EMRadius_3p_tight          = 0.0781     ;
  float c_0_trkAvgDist_3p_tight        = 0.356      ;
  float c_1_trkAvgDist_3p_tight        = 0.0455     ;
  float c_2_trkAvgDist_3p_tight        = -0.000357  ;
  float c_3_trkAvgDist_3p_tight        = 0.03       ;
  float EtOverPtLeadTrk_3p_tight       = 2.78   ;

  
  c_0_trkAvgDist_3p_medium *= 1.5;
  c_1_trkAvgDist_3p_medium *= 1.5;
  c_2_trkAvgDist_3p_medium *= 1.5;
  c_3_trkAvgDist_3p_medium *= 1.5;

  switch( level)
  {
  case( LOOSE):
      efSel.setParamSafeCuts(
          TrigTauEFSel::CutFunctor( c_0_EMRadius_1p_loose,   c_1_EMRadius_1p_loose,   c_2_EMRadius_1p_loose  ), // LowEMRad_OP(am1,a0,a1): am1/pt + a0 + a1*pt ==> c0/pt + c1 +c2*pt
          TrigTauEFSel::CutFunctor( c_0_EMRadius_3p_loose,   c_1_EMRadius_3p_loose,   c_2_EMRadius_3p_loose  ), // LowEMRad_MP
          TrigTauEFSel::CutFunctor( c_0_trkAvgDist_1p_loose, c_1_trkAvgDist_1p_loose, c_2_trkAvgDist_1p_loose), // LowTrkAvgD_OP
          TrigTauEFSel::CutFunctor( c_0_trkAvgDist_3p_loose, c_1_trkAvgDist_3p_loose, c_2_trkAvgDist_3p_loose), // Low TrkAvgD MP
          EtOverPtLeadTrk_1p_loose, EtOverPtLeadTrk_3p_loose,// LowEtOPt_SP/_MP
          c_3_EMRadius_1p_loose,    c_3_EMRadius_3p_loose,   // EMRad_SP/_MP
          c_3_trkAvgDist_1p_loose,  c_3_trkAvgDist_3p_loose, // TrkAvgD_SP/_MP,
          EtOverPtLeadTrk_1p_loose, EtOverPtLeadTrk_3p_loose,// EtOPt_SP/_MP
          1,                        nTrackMaxLoose,          // MinNTrack, MaxNtrack
          thresh,                   thresh,                  // MinEtCalib,  MinEtCalib
          60000);// EtThreshold
      break;
  case( MEDIUM1):
      efSel.setParamSafeCuts(
          TrigTauEFSel::CutFunctor( c_0_EMRadius_1p_medium,   c_1_EMRadius_1p_medium,   c_2_EMRadius_1p_medium  ), // LowEMRad_OP(am1,a0,a1): am1/pt + a0 + a1*pt ==> c0/pt + c1 +c2*pt
          TrigTauEFSel::CutFunctor( c_0_EMRadius_3p_medium,   c_1_EMRadius_3p_medium,   c_2_EMRadius_3p_medium  ), // LowEMRad_MP
          TrigTauEFSel::CutFunctor( c_0_trkAvgDist_1p_medium, c_1_trkAvgDist_1p_medium, c_2_trkAvgDist_1p_medium), // LowTrkAvgD_OP
          TrigTauEFSel::CutFunctor( c_0_trkAvgDist_3p_medium, c_1_trkAvgDist_3p_medium, c_2_trkAvgDist_3p_medium), // Low TrkAvgD MP
          EtOverPtLeadTrk_1p_medium, EtOverPtLeadTrk_3p_medium,// LowEtOPt_SP/_MP
          c_3_EMRadius_1p_medium,    c_3_EMRadius_3p_medium,   // EMRad_SP/_MP
          c_3_trkAvgDist_1p_medium,  c_3_trkAvgDist_3p_medium, // TrkAvgD_SP/_MP,
          EtOverPtLeadTrk_1p_medium, EtOverPtLeadTrk_3p_medium,// EtOPt_SP/_MP
          1,                         3,                        // MinNTrack, MaxNtrack
          thresh,                    thresh,                   // MinEtCalib,  MinEtCalib
          60000);// EtThreshold
      break;
  case( MEDIUM):
      efSel.setParamSafeCuts(
          TrigTauEFSel::CutFunctor( c_0_EMRadius_1p_medium,   c_1_EMRadius_1p_medium,   c_2_EMRadius_1p_medium  ), // LowEMRad_OP(am1,a0,a1): am1/pt + a0 + a1*pt ==> c0/pt + c1 +c2*pt
          TrigTauEFSel::CutFunctor( c_0_EMRadius_3p_medium,   c_1_EMRadius_3p_medium,   c_2_EMRadius_3p_medium  ), // LowEMRad_MP
          TrigTauEFSel::CutFunctor( c_0_trkAvgDist_1p_medium, c_1_trkAvgDist_1p_medium, c_2_trkAvgDist_1p_medium), // LowTrkAvgD_OP
          TrigTauEFSel::CutFunctor( c_0_trkAvgDist_3p_medium, c_1_trkAvgDist_3p_medium, c_2_trkAvgDist_3p_medium), // Low TrkAvgD MP
          EtOverPtLeadTrk_1p_medium, EtOverPtLeadTrk_3p_medium,// LowEtOPt_SP/_MP
          c_3_EMRadius_1p_medium,    c_3_EMRadius_3p_medium,   // EMRad_SP/_MP
          c_3_trkAvgDist_1p_medium,  c_3_trkAvgDist_3p_medium, // TrkAvgD_SP/_MP,
          EtOverPtLeadTrk_1p_medium, EtOverPtLeadTrk_3p_medium,// EtOPt_SP/_MP
          1,                         6,                        // MinNTrack, MaxNtrack
          thresh,                    thresh,                   // MinEtCalib,  MinEtCalib
          60000);// EtThreshold
      break;
  case( TIGHT):
      efSel.setParamSafeCuts(
          TrigTauEFSel::CutFunctor( c_0_EMRadius_1p_tight,   c_1_EMRadius_1p_tight,   c_2_EMRadius_1p_tight  ), // LowEMRad_OP(am1,a0,a1): am1/pt + a0 + a1*pt ==> c0/pt + c1 +c2*pt
          TrigTauEFSel::CutFunctor( c_0_EMRadius_3p_tight,   c_1_EMRadius_3p_tight,   c_2_EMRadius_3p_tight  ), // LowEMRad_MP
          TrigTauEFSel::CutFunctor( c_0_trkAvgDist_1p_tight, c_1_trkAvgDist_1p_tight, c_2_trkAvgDist_1p_tight), // LowTrkAvgD_OP
          TrigTauEFSel::CutFunctor( c_0_trkAvgDist_3p_tight, c_1_trkAvgDist_3p_tight, c_2_trkAvgDist_3p_tight), // Low TrkAvgD MP
          EtOverPtLeadTrk_1p_tight, EtOverPtLeadTrk_3p_tight,// LowEtOPt_SP/_MP
          c_3_EMRadius_1p_tight,    c_3_EMRadius_3p_tight,   // EMRad_SP/_MP
          c_3_trkAvgDist_1p_tight,  c_3_trkAvgDist_3p_tight, // TrkAvgD_SP/_MP,
          EtOverPtLeadTrk_1p_tight, EtOverPtLeadTrk_3p_tight,// EtOPt_SP/_MP
          1,                        6,                       // MinNTrack, MaxNtrack
          thresh,                   thresh,                  // MinEtCalib,  MinEtCalib
          60000);// EtThreshold
  default:
    break;
  }

  return;
 }

//=============================================================================
// MENU1662 corresponds to TrigTauHypo-00-02-34, 
// https://svnweb.cern.ch/trac/atlasoff/browser/Trigger/TrigHypothesis/TrigTauHypo/tags/TrigTauHypo-00-02-34
//=============================================================================
void
TrigTauSelectionManager::setTauMenu1662( TrigTauFlags::TRIGGERMENU type,
                                         CUTLEVEL level)
{
  switch (type)
  {
  case TrigTauFlags::TAUNOCUT:
    l1Sel.setCuts(0, 5000, 0, 0, 0, 0);
    l2TrkSel.setCuts( 1000, -1, -1, -1, 7500, 0.1, 1500, 0.15, 0.30, 0, false);
    l2CalSel.setCuts(0, 0, 0, 0.0, 0.0, 0.0, 0);
    efSel.setCuts(0, 100, 100., 1000., -10., 0.);
    break;
    
  case TrigTauFlags::TAU12:
    l1Sel.setCuts(0, 5000, 0, 0, 0, 0);
    l2TrkSel.setCoreFromLeading(true);
    setParametrizedEFCuts( efSel, level, 12000);
    switch(level)
    {
    case LOOSE:
      l2TrkSel.setCuts(10, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false,
                       0.1, 0, 1);
      l2CalSel.setNewCuts( 4900, 0.0875, 0.141336);
      break;
    case MEDIUM:
      l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 0, 0.15, 0.30, 0, false,
                       0.1, 0, 1);
      l2CalSel.setNewCuts( 4900, 0.0837, 0.139087);
      break;
    default :
      cout << "FIXME TrigTauSelectionManager has internal problem with menu "
           << TrigTauFlags::menuName(type) 
           << " and level " << level << endl; break;
    }
    break;
    
  case TrigTauFlags::TAU16:
    l1Sel.setCuts(0, 6000, 0, 0, 0, 0);
    l2TrkSel.setCoreFromLeading(true);
    setParametrizedEFCuts( efSel, level, 16000);
    switch(level)
    {
    case LOOSE:
      l2TrkSel.setCuts(10, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false,
                       0.1, 0, 1);
      l2CalSel.setNewCuts( 7800, 0.084327, 0.137957);
      break;
    case MEDIUM:
      l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false,
                       0.1, 0., 1);
      l2CalSel.setNewCuts( 7800, 0.08060, 0.135664);
      break;
    case MEDIUM1:
      l2TrkSel.setCuts(4, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false,
                       0.1, 0., 1);
      l2CalSel.setNewCuts( 7800, 0.08060, 0.135664);
      break;
    default :
      cout << "FIXME TrigTauSelectionManager has internal problem with menu "
           << TrigTauFlags::menuName(type) 
           << " and level " << level << endl; break;
    }
    break;
    
  case TrigTauFlags::TAU20:
    l1Sel.setCuts(0, 8000, 0, 0, 0, 0);
    l2TrkSel.setCoreFromLeading(true);
    setParametrizedEFCuts( efSel, level, 20000);
    switch(level)
    {
    case LOOSE:
      l2TrkSel.setCuts(10, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false,
                       0.1, 0, 1);
      l2CalSel.setNewCuts( 9800, 0.08320, 0.137957);
      break;
    case MEDIUM:
      l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false,
                       0.05, 0., 1);
      l2CalSel.setNewCuts( 9800, 0.0795288, 0.135664);
      break;
    case MEDIUM1:
      l2TrkSel.setCuts(4, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false,
                       0.05, 0., 1);
      l2CalSel.setNewCuts( 9800, 0.0795288, 0.135664);
      break;
    default:
      cout << "FIXME TrigTauSelectionManager has internal problem with menu "
           << TrigTauFlags::menuName(type) 
           << " and level " << level << endl;break;
    }
    break;
    
  case TrigTauFlags::TAU29:
    l1Sel.setCuts(0, 11000, 0, 0, 0, 0);
    l2TrkSel.setCoreFromLeading(true);
    setParametrizedEFCuts( efSel, level, 29000);
    switch(level)
    {
    case LOOSE:
      l2TrkSel.setCuts(10, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false,
                       0.1, 0, 1);
      l2CalSel.setNewCuts( 16900, 0.07279, 0.124742);
      break;
    case MEDIUM:
      l2TrkSel.setCuts( 7, -1, -1, 99, 7500, 0.1, 0, 0, 0, 0, false,
                        0.05, 0, 1);
      l2CalSel.setNewCuts( 16900, 0.067706, 0.12162);
      break;
    case MEDIUM1:
      l2TrkSel.setCuts( 4, -1, -1, 99, 7500, 0.1, 0, 0, 0, 0, false,
                        0.05, 0, 1);
      l2CalSel.setNewCuts( 16900, 0.067706, 0.12162);
      break;
    default :
      cout << "FIXME TrigTauSelectionManager has internal problem with menu "
           << TrigTauFlags::menuName(type) 
           << " and level " << level << endl;break;
    }
    break;
    
  case TrigTauFlags::TAU38:
    l1Sel.setCuts(0, 11000, 0, 0, 0, 0);
    l2TrkSel.setCoreFromLeading(true);
    setParametrizedEFCuts( efSel, level, 38000);
    switch(level)
    {
    case LOOSE:
      l2TrkSel.setCuts(10, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false,
                       0.1, 0, 1);
      l2CalSel.setNewCuts( 22700, 0.06991, 0.119034);
      break;
    case MEDIUM:
      l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false,
                       0.05, 0., 1);
      l2CalSel.setNewCuts( 22700, 0.064107, 0.118296);
      break;
    default :
      cout << "FIXME TrigTauSelectionManager has internal problem with menu "
           << TrigTauFlags::menuName(type) 
           << " and level " << level << endl;break;
    }
    break;

  case TrigTauFlags::TAU50:
    l1Sel.setCuts(0, 20000, 0, 0, 0, 0);
    l2TrkSel.setCoreFromLeading(true);
    setParametrizedEFCuts( efSel, level, 50000);
    switch(level)
    {
    case LOOSE:
      l2TrkSel.setCuts(10, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false,
                       0.1, 0, 1);
      l2CalSel.setNewCuts( 31900, 0.0603379, 0.101723);
      break;
    case MEDIUM:
      l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false,
                       0.05, 0, 1);
      l2CalSel.setNewCuts( 31900, 0.05635, 0.11028);
      break;
    default :
      cout << "FIXME TrigTauSelectionManager has internal problem with menu "
           << TrigTauFlags::menuName(type) 
           << " and level " << level << endl;break;
    }
    break;
    
  case TrigTauFlags::TAU84:
    l1Sel.setCuts(0, 20000, 0, 0, 0, 0);
    l2TrkSel.setCoreFromLeading(true);
    setParametrizedEFCuts( efSel, level, 84000);
    switch(level)
    {
    case LOOSE:
      l2TrkSel.setCuts(10, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false,
                       0.1, 0, 1);
      l2CalSel.setNewCuts( 51000, 0.06033, 0.1);
      break;
    case MEDIUM:
      l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false,
                       0.02, 0, 1);
      l2CalSel.setNewCuts( 51000, 0.05634, 0.09);
      break;
    default :
      cout << "FIXME TrigTauSelectionManager has internal problem with menu "
           << TrigTauFlags::menuName(type) 
           << " and level " << level << endl;break;
    }
    break;
    
  case TrigTauFlags::TAU100:
    l1Sel.setCuts(0, 50000, 0, 0, 0, 0);
    l2TrkSel.setCoreFromLeading(true);
    setParametrizedEFCuts( efSel, level, 100000);
    switch(level)
    {
    case LOOSE:
      l2TrkSel.setCuts(10, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false,
                       0.1, 0, 1);
      l2CalSel.setNewCuts( 60000, 0.0603379, 0.09);
      break;
    case MEDIUM:
      l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false,
                       0.02, 0, 1);
      l2CalSel.setNewCuts( 60000, 0.05635, 0.08);
      break;
    default :
      cout << "FIXME TrigTauSelectionManager has internal problem with menu "
           << TrigTauFlags::menuName(type) 
           << " and level " << level << endl;break;
    }
    break;
    
  case TrigTauFlags::TAU125:
    l1Sel.setCuts(0, 50000, 0, 0, 0, 0);
    l2TrkSel.setCoreFromLeading(true);
    setParametrizedEFCuts( efSel, level, 125000);
    switch(level)
    {
    case LOOSE:
      l2TrkSel.setCuts(10, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false,
                       0.1, 0, 1);
      l2CalSel.setNewCuts( 75000, 0.0603379, 0.08);
      break;
    case MEDIUM:
      l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false,
                       0.02, 0, 1);
      l2CalSel.setNewCuts( 75000, 0.05635, 0.07);
      break;
    default :
      cout << "FIXME TrigTauSelectionManager has internal problem with menu "
           << TrigTauFlags::menuName(type) 
           << " and level " << level << endl;
      break;
    }
    break;
  default :
    cout << ":setTauMenu MENU1662 can not deal with menu "
         << TrigTauFlags::menuName(type) << endl;
    break;
  }
}


//=============================================================================
// TrigTauHypo-00-02-42
// https://svnweb.cern.ch/trac/atlasoff/browser/Trigger/TrigHypothesis/TrigTauHypo/tags/TrigTauHypo-00-02-42
//=============================================================================
void
TrigTauSelectionManager::setTauMenu1666( TrigTauFlags::TRIGGERMENU type,
                                         CUTLEVEL level)
{
  switch (type)
  {
  case TrigTauFlags::TAUNOCUT:
    l1Sel.setCuts(0, 5000, 0, 0, 0, 0);
    l2TrkSel.setCuts( 1000, -1, -1, -1, 7500, 0.1, 1500, 0.15, 0.30, 0, false);
    l2CalSel.setCuts(0, 0, 0, 0.0, 0.0, 0.0, 0);
    efSel.setCuts(0, 100, 100., 1000., -10., 0.);
    break;
    
  case TrigTauFlags::TAU12:
    l1Sel.setCuts(0, 5000, 0, 0, 0, 0);
    l2TrkSel.setCoreFromLeading(true);
    setParametrizedEFCuts( efSel, level, 12000);
    switch(level)
    {
    case LOOSE:
      l2TrkSel.setCuts(10, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false,
                       0.1, 0, 1);
      l2CalSel.setNewCuts( 8100, 0.09, 0.16);
      break;
    case MEDIUM:
      l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 0, 0.15, 0.30, 0, false,
                       0.05, 0, 1);
      l2CalSel.setNewCuts( 8100, 0.09, 0.15);
      break;
    default :
      cout << "FIXME TrigTauSelectionManager has internal problem with menu "
           << TrigTauFlags::menuName(type) 
           << " and level " << level << endl; break;
    }
    break;
    
  case TrigTauFlags::TAU16:
    l1Sel.setCuts(0, 6000, 0, 0, 0, 0);
    l2TrkSel.setCoreFromLeading(true);
    setParametrizedEFCuts( efSel, level, 16000);
    switch(level)
    {
    case LOOSE:
      l2TrkSel.setCuts(10, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false,
                       0.1, 0, 1);
      l2CalSel.setNewCuts( 11300, 0.08, 0.15);

      break;
    case MEDIUM:
      l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false,
                       0.05, 0., 1);
      l2CalSel.setNewCuts( 11300, 0.08, 0.14);
      break;
    case MEDIUM1:
      l2TrkSel.setCuts(4, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false,
                       0.05, 0., 1);
      l2CalSel.setNewCuts( 11300, 0.08, 0.14);
      break;
    default :
      cout << "FIXME TrigTauSelectionManager has internal problem with menu "
           << TrigTauFlags::menuName(type) 
           << " and level " << level << endl; break;
    }
    break;
    
  case TrigTauFlags::TAU20:
    l1Sel.setCuts(0, 8000, 0, 0, 0, 0);
    l2TrkSel.setCoreFromLeading(true);
    setParametrizedEFCuts( efSel, level, 20000);
    switch(level)
    {
    case LOOSE:
      l2TrkSel.setCuts(10, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false,
                       0.1, 0, 1);
      l2CalSel.setNewCuts( 13800, 0.08, 0.13);
      break;
    case MEDIUM:
      l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false,
                       0.05, 0., 1);
      l2CalSel.setNewCuts( 13800, 0.07, 0.13);
      break;
    case MEDIUM1:
      l2TrkSel.setCuts(4, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false,
                       0.05, 0., 1);
      l2CalSel.setNewCuts( 13800, 0.07, 0.13);
      break;
    default:
      cout << "FIXME TrigTauSelectionManager has internal problem with menu "
           << TrigTauFlags::menuName(type) 
           << " and level " << level << endl;break;
    }
    break;
    
  case TrigTauFlags::TAU29:
    l1Sel.setCuts(0, 11000, 0, 0, 0, 0);
    l2TrkSel.setCoreFromLeading(true);
    setParametrizedEFCuts( efSel, level, 29000);
    switch(level)
    {
    case LOOSE:
      l2TrkSel.setCuts(10, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false,
                       0.1, 0, 1);
      l2CalSel.setNewCuts( 21600, 0.06, 0.12);
      break;
    case MEDIUM:
      l2TrkSel.setCuts( 7, -1, -1, 99, 7500, 0.1, 0, 0, 0, 0, false,
                        0.05, 0, 1);
      l2CalSel.setNewCuts( 21600, 0.06, 0.12);
      break;
    case MEDIUM1:
      l2TrkSel.setCuts( 4, -1, -1, 99, 7500, 0.1, 0, 0, 0, 0, false,
                        0.05, 0, 1);
      l2CalSel.setNewCuts( 21600, 0.06, 0.12);
      break;
    default :
      cout << "FIXME TrigTauSelectionManager has internal problem with menu "
           << TrigTauFlags::menuName(type) 
           << " and level " << level << endl;break;
    }
    break;
    
  case TrigTauFlags::TAU38:
    l1Sel.setCuts(0, 11000, 0, 0, 0, 0);
    l2TrkSel.setCoreFromLeading(true);
    setParametrizedEFCuts( efSel, level, 38000);
    switch(level)
    {
    case LOOSE:
      l2TrkSel.setCuts(10, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false,
                       0.1, 0, 1);
      l2CalSel.setNewCuts( 26900, 0.06, 0.12);
      break;
    case MEDIUM:
      l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false,
                       0.05, 0., 1);
      l2CalSel.setNewCuts( 26900, 0.05, 0.09);
      break;
    default :
      cout << "FIXME TrigTauSelectionManager has internal problem with menu "
           << TrigTauFlags::menuName(type) 
           << " and level " << level << endl;break;
    }
    break;

  case TrigTauFlags::TAU50:
    l1Sel.setCuts(0, 20000, 0, 0, 0, 0);
    l2TrkSel.setCoreFromLeading(true);
    setParametrizedEFCuts( efSel, level, 50000);
    switch(level)
    {
    case LOOSE:
      l2TrkSel.setCuts(10, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false,
                       0.1, 0, 1);
      l2CalSel.setNewCuts( 42400, 0.05, 0.09);
      break;
    case MEDIUM:
      l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false,
                       0.05, 0, 1);
      l2CalSel.setNewCuts( 42400, 0.05, 0.09);
      break;
    default :
      cout << "FIXME TrigTauSelectionManager has internal problem with menu "
           << TrigTauFlags::menuName(type) 
           << " and level " << level << endl;break;
    }
    break;
    
  case TrigTauFlags::TAU84:
    l1Sel.setCuts(0, 20000, 0, 0, 0, 0);
    l2TrkSel.setCoreFromLeading(true);
    setParametrizedEFCuts( efSel, level, 84000);
    switch(level)
    {
    case LOOSE:
      l2TrkSel.setCuts(10, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false,
                       0.1, 0, 1);
      l2CalSel.setNewCuts( 64700, 0.05, 0.08);
      break;
    case MEDIUM:
      l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false,
                       0.02, 0, 1);
      l2CalSel.setNewCuts( 64700, 0.04, 0.08);
      break;
    default :
      cout << "FIXME TrigTauSelectionManager has internal problem with menu "
           << TrigTauFlags::menuName(type) 
           << " and level " << level << endl;break;
    }
    break;
    
  case TrigTauFlags::TAU100:
    l1Sel.setCuts(0, 50000, 0, 0, 0, 0);
    l2TrkSel.setCoreFromLeading(true);
    setParametrizedEFCuts( efSel, level, 100000);
    switch(level)
    {
    case LOOSE:
      l2TrkSel.setCuts(10, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false,
                       0.1, 0, 1);
      l2CalSel.setNewCuts( 81600, 0.05, 0.08);
      break;
    case MEDIUM:
      l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false,
                       0.02, 0, 1);
      l2CalSel.setNewCuts( 81600, 0.04, 0.08);
      break;
    case MEDIUM1:
      l2TrkSel.setCuts(4, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false,//lowered max N trk to 4
                       0.02, 0, 1);
      l2CalSel.setNewCuts( 81600, 0.04, 0.08);
      break;
    default :
      cout << "FIXME TrigTauSelectionManager has internal problem with menu "
           << TrigTauFlags::menuName(type) 
           << " and level " << level << endl;break;
    }
    break;
    
  case TrigTauFlags::TAU125:
    l1Sel.setCuts(0, 50000, 0, 0, 0, 0);
    l2TrkSel.setCoreFromLeading(true);
    setParametrizedEFCuts( efSel, level, 125000);
    switch(level)
    {
    case LOOSE:
      l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false,
                       0.1, 0, 1);
      l2CalSel.setNewCuts( 81600, 0.04, 0.08);
      break;
    case MEDIUM:
      l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false,
                       0.02, 0, 1);
      l2CalSel.setNewCuts( 81600, 0.04, 0.08);
      break;
    default :
      cout << "FIXME TrigTauSelectionManager has internal problem with menu "
           << TrigTauFlags::menuName(type) 
           << " and level " << level << endl;
      break;
    }
    break;
  default :
    cout << ":setTauMenu MENU1666 can not deal with menu "
         << TrigTauFlags::menuName(type) << endl;
    break;
  }
}

//=============================================================================
// For now TrigTauHypo-00-03-02
// https://svnweb.cern.ch/trac/atlasoff/browser/Trigger/TrigHypothesis/TrigTauHypo/tags/TrigTauHypo-00-03-02
//=============================================================================
void
TrigTauSelectionManager::setTauMenuTest( TrigTauFlags::TRIGGERMENU type,
                                         CUTLEVEL level)
{
  switch (type)
  {
  case TrigTauFlags::TAUNOCUT:
    l1Sel.setCuts(0, 5000, 0, 0, 0, 0);
    l2TrkSel.setCuts( 1000, -1, -1, -1, 7500, 0.1, 1500, 0.15, 0.30, 0, false);
    l2CalSel.setCuts(0, 0, 0, 0.0, 0.0, 0.0, 0);
    efSel.setCuts(0, 100, 100., 1000., -10., 0.);
    break;
    
  case TrigTauFlags::TAU12:
    l1Sel.setCuts(0, 5000, 0, 0, 0, 0);
    l2TrkSel.setCoreFromLeading(true);
    setParametrizedEFCuts_201104( efSel, level, 12000);
    switch(level)
    {
    case LOOSE:
      l2TrkSel.setCuts(10, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false,
                       0.1, 0, 1);
      l2CalSel.setNewCuts( 9400, 0.1074, 0.1411);
      break;
    case MEDIUM:
      l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 0, 0.15, 0.30, 0, false,
                       0.05, 0, 1);
      l2CalSel.setNewCuts( 9400, 0.0878, 0.1265);
      break;
    default :
      cout << "FIXME TrigTauSelectionManager has internal problem with menu "
           << TrigTauFlags::menuName(type) 
           << " and level " << level << endl; break;
    }
    break;
    
  case TrigTauFlags::TAU16:
    l1Sel.setCuts(0, 6000, 0, 0, 0, 0);
    l2TrkSel.setCoreFromLeading(true);
    setParametrizedEFCuts_201104( efSel, level, 16000);
    switch(level)
    {
    case LOOSE:
      l2TrkSel.setCuts(10, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false,
                       0.1, 0, 1);
      l2CalSel.setNewCuts( 12400, 0.0996, 0.1310);

      break;
    case MEDIUM:
      l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false,
                       0.05, 0., 1);
      l2CalSel.setNewCuts( 12400, 0.0809, 0.1189);
      break;
    case MEDIUM1:
      l2TrkSel.setCuts(4, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false,
                       0.05, 0., 1);
      l2CalSel.setNewCuts( 12400, 0.0809, 0.1189);
      break;
    default :
      cout << "FIXME TrigTauSelectionManager has internal problem with menu "
           << TrigTauFlags::menuName(type) 
           << " and level " << level << endl; break;
    }
    break;
    
  case TrigTauFlags::TAU20:
    l1Sel.setCuts(0, 8000, 0, 0, 0, 0);
    l2TrkSel.setCoreFromLeading(true);
    setParametrizedEFCuts_201104( efSel, level, 20000);
    switch(level)
    {
    case LOOSE:
      l2TrkSel.setCuts(10, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false,
                       0.1, 0, 1);
      l2CalSel.setNewCuts( 16700, 0.0833, 0.1179);
      break;
    case MEDIUM:
      l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false,
                       0.05, 0., 1);
      l2CalSel.setNewCuts( 16700, 0.0696, 0.1049);
      break;
    case MEDIUM1:
      l2TrkSel.setCuts(4, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false,
                       0.05, 0., 1);
      l2CalSel.setNewCuts( 16700, 0.0696, 0.1049);
      break;
    default:
      cout << "FIXME TrigTauSelectionManager has internal problem with menu "
           << TrigTauFlags::menuName(type) 
           << " and level " << level << endl;break;
    }
    break;
    
  case TrigTauFlags::TAU29:
    l1Sel.setCuts(0, 11000, 0, 0, 0, 0);
    l2TrkSel.setCoreFromLeading(true);
    setParametrizedEFCuts_201104( efSel, level, 29000);
    switch(level)
    {
    case LOOSE:
      l2TrkSel.setCuts(10, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false,
                       0.1, 0, 1);
      l2CalSel.setNewCuts( 23200, 0.0705, 0.1015);
      break;
    case MEDIUM:
      l2TrkSel.setCuts( 7, -1, -1, 99, 7500, 0.1, 0, 0, 0, 0, false,
                        0.05, 0, 1);
      l2CalSel.setNewCuts( 23200, 0.0618, 0.0962);
      break;
    case MEDIUM1:
      l2TrkSel.setCuts( 4, -1, -1, 99, 7500, 0.1, 0, 0, 0, 0, false,
                        0.05, 0, 1);
      l2CalSel.setNewCuts( 23200, 0.0618, 0.0962);
      break;
    default :
      cout << "FIXME TrigTauSelectionManager has internal problem with menu "
           << TrigTauFlags::menuName(type) 
           << " and level " << level << endl;break;
    }
    break;
    
  case TrigTauFlags::TAU38:
    l1Sel.setCuts(0, 11000, 0, 0, 0, 0);
    l2TrkSel.setCoreFromLeading(true);
    setParametrizedEFCuts_201104( efSel, level, 38000);
    switch(level)
    {
    case LOOSE:
      l2TrkSel.setCuts(10, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false,
                       0.1, 0, 1);
      l2CalSel.setNewCuts( 28300, 0.0702, 0.0847);
      break;
    case MEDIUM:
      l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false,
                       0.05, 0., 1);
      l2CalSel.setNewCuts( 28300, 0.0521, 0.0823);
      break;
    default :
      cout << "FIXME TrigTauSelectionManager has internal problem with menu "
           << TrigTauFlags::menuName(type) 
           << " and level " << level << endl;break;
    }
    break;

  case TrigTauFlags::TAU50:
    l1Sel.setCuts(0, 20000, 0, 0, 0, 0);
    l2TrkSel.setCoreFromLeading(true);
    setParametrizedEFCuts_201104( efSel, level, 50000);
    switch(level)
    {
    case LOOSE:
      l2TrkSel.setCuts(10, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false,
                       0.1, 0, 1);
      l2CalSel.setNewCuts( 44300, 0.0587, 0.0779);
      break;
    case MEDIUM:
      l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false,
                       0.05, 0, 1);
      l2CalSel.setNewCuts( 44300, 0.0495, 0.0726);
      break;
    default :
      cout << "FIXME TrigTauSelectionManager has internal problem with menu "
           << TrigTauFlags::menuName(type) 
           << " and level " << level << endl;break;
    }
    break;
    
  case TrigTauFlags::TAU84:
    l1Sel.setCuts(0, 20000, 0, 0, 0, 0);
    l2TrkSel.setCoreFromLeading(true);
    setParametrizedEFCuts_201104( efSel, level, 84000);
    switch(level)
    {
    case LOOSE:
      l2TrkSel.setCuts(10, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false,
                       0.1, 0, 1);
      l2CalSel.setNewCuts( 72100, 0.0494, 0.0686);
      break;
    case MEDIUM:
      l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false,
                       0.02, 0, 1);
      l2CalSel.setNewCuts( 72100, 0.0445, 0.0663);
      break;
    default :
      cout << "FIXME TrigTauSelectionManager has internal problem with menu "
           << TrigTauFlags::menuName(type) 
           << " and level " << level << endl;break;
    }
    break;
    
  case TrigTauFlags::TAU100:
    l1Sel.setCuts(0, 50000, 0, 0, 0, 0);
    l2TrkSel.setCoreFromLeading(true);
    setParametrizedEFCuts_201104( efSel, level, 100000);
    switch(level)
    {
    case LOOSE:
      l2TrkSel.setCuts(10, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false,
                       0.1, 0, 1);
      l2CalSel.setNewCuts( 88500, 0.0481, 0.0687);
      break;
    case MEDIUM:
      
      l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false,
		       0.02, 0, 1);
      l2CalSel.setNewCuts( 88500, 0.0426, 0.064);
      break;
    case MEDIUM1:
      l2TrkSel.setCuts(4, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false,//lowered max N trk to 4
                       0.02, 0, 1);
      l2CalSel.setNewCuts( 88500, 0.0426, 0.064);
      break;
    default :
      cout << "FIXME TrigTauSelectionManager has internal problem with menu "
           << TrigTauFlags::menuName(type) 
           << " and level " << level << endl;break;
    }
    break;
    
  case TrigTauFlags::TAU125:
    l1Sel.setCuts(0, 50000, 0, 0, 0, 0);
    l2TrkSel.setCoreFromLeading(true);
    setParametrizedEFCuts_201104( efSel, level, 125000);
    switch(level)
    {
    case LOOSE:
      l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false,
                       0.1, 0, 1);
      l2CalSel.setNewCuts( 104300, 0.0463, 0.0654);
      break;
    case MEDIUM:
      l2TrkSel.setCuts(7, -1, -1, 99, 7500, 0.1, 1500, 0.15, 0.30, 0, false,
                       0.02, 0, 1);
      l2CalSel.setNewCuts( 104300, 0.0414, 0.0621);
      break;
    default :
      cout << "FIXME TrigTauSelectionManager has internal problem with menu "
           << TrigTauFlags::menuName(type) 
           << " and level " << level << endl;
      break;
    }
    break;
    
  default :
    cout << ":setTauMenu MENUTEST can not deal with menu "
         << TrigTauFlags::menuName(type) << endl;
    break;
  }
}

// Cuts are:
// L1:
//   EmEnergy, TauEnergy, EmIsol, HdIsol, EmCore, HdCore
//
// L2Cal:
//   Et, EtCalib, nStrips, stripWidth, EMRad, IsoFrac, EMEnWidth2 , EtNar
//
// L2Cal (new):
//   RawEtNor, EMRadiusSP, EMRadiusMP 
//
// L2Trk:
//   NtrkMax, NtrkIsol, NtrkSlow, NtrkMatch, 
//     maxPtSlow, dRIsol, mPt, dRMatched, dRtot, leadingPt, chargeCut,
//     sumPtRatio,trkEmEt, NtrkMin
//
// EF:
//   NTrackMin, NTrackMax, EMRadiusMax, IsoFracMax
//     EMFracMin, PtMaxTrackMin, EtCalibMin, EMEtMin
//
// EF param:
//   LowEMRadSP, LowEMRadMP, LowTrkAvgDistSP, LowTrkAvgDistMP,
//   LowEtOverPtLeadTrkSP, LowEtOverPtLeadTrkMP, HighEMRadSP, HighEMRadMP,
//   HighTrkAvgDistSP, HighTrkAvgDistMP, HighEtOverPtLeadTrkSP,
//   HighEtOverPtLeadTrkMP, ETThresh, EtCalibMinSP, EtCalibMinMP,
//   NTrackMin, NTrackMax


void
TrigTauSelectionManager::printSettings() const {
  cout << "TTP Selection for ";
  switch (_menuVersion){
  case MENU1206   : cout << " 12.0.6 "; break;
  case MENUJUNE06 : cout << " June07 "; break;
  case MENUJUNE07 : cout << " June07 "; break;
  case MENU13020  : cout << " 13.0.20 "; break;
  case MENUAUG07  : cout << " 13.0.20-dev "; break;
  case MENU13030pre : cout << " 13.0.30.3 "; break;
  case MENU1400 : cout << " 14.0.0 "; break;
  case MENU1520test : cout << " 15.2.0 -test L1 EM scale "; break;
  case MENU1600 : cout << " 16.0.0 "; break;
  case MENU1662 : cout << " 16.6.2, TrigTauHypo-00-02-34"; break;
  case MENU1666 : cout << " 16.6.2.6, TrigTauHypo-00-02-42, TAU125_MEDIUM temporarily modified"; break;
  case MENUTEST : cout << " Test menu, TrigTauHypo-00-03-02"; break;
  default : cout << " UNKNOWN "; break;
  }
  cout << "menu \n requested signature was "<< TrigTauFlags::menuName(_givenType) << ", used " <<TrigTauFlags::menuName(_usedType) << " with ";
  switch (_cutLevel){
  case DEFAULT : cout << " DEFAULT "; break;
  case SOFT : cout << " SOFT " ; break;
  case LOOSE : cout << " LOOSE " ; break;
  case MEDIUM : cout << " MEDIUM " ; break;
  case MEDIUM1 : cout << " MEDIUM1 " ; break;
  case TIGHT : cout << " TIGHT " ; break;

  }
  cout << " level";
  if( l2CalOnly ) cout << " L2Trk is OFF";
  if( l2TrkOnly ) cout << " L2Cal is OFF";
  cout << endl;


}

void
TrigTauSelectionManager::print() const {
  l1Sel.print();
  l1METSel.print();
  l2CalSel.print();
  l2TrkSel.print();
  efSel.print();
}

// Selector functions
bool
TrigTauSelectionManager::passedL1(const TrigTauTrigChain& chain) {
  if (chain.L1 == 0) return false;
  return l1Sel.passed(*chain.L1) && l1METSel.passed(*chain.L1MET);
}

bool
TrigTauSelectionManager::passedL2Cal(const TrigTauTrigChain& chain) {
  if (chain.L2 == 0) return false;
  return (passedL1(chain) &&
	  l2CalSel.passed(*chain.L2));
}

bool
TrigTauSelectionManager::passedL2Trk(const TrigTauTrigChain& chain) {
  if (chain.L2 == 0) return false;
  return (passedL1(chain) &&
	  l2TrkSel.passed(*chain.L2));
}

bool
TrigTauSelectionManager::passedL2(const TrigTauTrigChain& chain) {
  if (chain.L2 == 0) return false;

  if (l2CalOnly)
    return (passedL1(chain) &&
	    l2CalSel.passed(*chain.L2));

  if (l2TrkOnly)
    return (passedL1(chain) &&
	    l2TrkSel.passed(*chain.L2));
  
    return (passedL1(chain) &&
	    l2CalSel.passed(*chain.L2) &&
	    l2TrkSel.passed(*chain.L2));
}

bool
TrigTauSelectionManager::passedEF(const TrigTauTrigChain& chain) {
  if (chain.EF == 0) return false;
  return (passedL2(chain) &&
	  efSel.passed(*chain.EF));
}

void TrigTauSelectionManager::setDebug(bool val){
  l1Sel.setDebug(val);
  l1METSel.setDebug(val);
  l2CalSel.setDebug(val);
  l2TrkSel.setDebug(val);
  efSel.setDebug(val);
}

