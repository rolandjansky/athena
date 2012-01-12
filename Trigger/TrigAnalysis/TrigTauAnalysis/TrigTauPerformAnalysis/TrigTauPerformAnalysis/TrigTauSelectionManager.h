/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TRIGTAUSELECTIONMANAGER_H
#define _TRIGTAUSELECTIONMANAGER_H

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigTauSelectionManager.h
/// Package : TrigTauPerformAnalysis
/// Author  : Eric Torrence (University of Oregon)
/// Created : February 2007
///
/// DESCRIPTION:
///           Class to manage the individual L1/L2/EF selectors.
///           Can also be used to define complete trigger menus
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <vector>

#include "TrigTauPerformAnalysis/TrigTauL1Sel.h"
#include "TrigTauPerformAnalysis/TrigTauL2CalSel.h"
#include "TrigTauPerformAnalysis/TrigTauL2TrkSel.h"
#include "TrigTauPerformAnalysis/TrigTauEFSel.h"

#include "TrigTauPerformAnalysis/TrigTauMETL1Sel.h"


class TrigTauTrigChain;

class TrigTauSelectionManager {

 public:
  TrigTauSelectionManager();
  virtual ~TrigTauSelectionManager();

  enum CUTLEVEL{    // level of cuts, applicable from MENU1400 only
    DEFAULT, // take cut level according to the TrigTauFlags. other options allows to modify it
    SOFT,
    LOOSE,
    MEDIUM,
    MEDIUM1,
    TIGHT
  };

  enum MENUVERSION {
    MENU1206,     // Default 12.0.6 menus
    MENUJUNE06,   // June T/P week menus
    MENUJUNE07,   // June T/P week menus (properly named)
    MENU13020,    // Default 13.0.20 menus
    MENUAUG07,    // Mostly 13.0.20 cuts, June L2Cal cuts, trk+EM energy
    MENU13030pre, // 13.0.30 cuts
    MENU1400,     // 14.0.0 cuts
    MENU1520test, // optimized  on 15.1.0, introduced in 15.X.0 cuts
    MENU1560,     // 15.6.0, simple L1 calibration
    MENU1600,     // 16.0.0, HLT rejection
    MENU1662,     // 16.6.2, Et parametrized EF cuts
    MENU1666,     // 16.6.2.6, Et parametrized EF cuts-tight selection
    MENUTEST      // Test menu
  };

  // Use TrigTauFlags::TRIGGERMENU to specify menus 
  void setTauMenu(TrigTauFlags::TRIGGERMENU type, CUTLEVEL level= DEFAULT );


  // Menu version defaults to 12.0.6.  Must be set differently
  // once before using setTauMenu to set generic menus.
  void setMenuVersion( MENUVERSION version) {_menuVersion = version;}

  // Use HLT trigger bits to select HLT ROIs instead of emulating trigger.
  // Note, MENUVERSION must exactly match the bit requested, as the level
  // modifier is not considered.  This may be called after
  // setTauMenu, but may be toggeled on/off during execution.
  void useHLTTriggerBits(bool useBits=true);

  //  void setMETMenu(TRIGMETMENU type);

  // Use only calo selection at L2 (default is Trk+Calo)
  void setL2CalOnly(bool calOnly=true) {l2CalOnly = calOnly;}
  void setL2TrkOnly(bool trkOnly=true) {l2TrkOnly = trkOnly;}

  // Shortcut to the l1METSel function
  void setL1MET(float METCut=0.) {l1METSel.setCuts(METCut);}

  void printSettings() const;
  void print() const;
  void setDebug(bool val);

  // Select by chain (Note: passing L2 also requires passing L1)
  bool passedL1(const TrigTauTrigChain& chain);
  bool passedL2(const TrigTauTrigChain& chain);
  bool passedL2Cal(const TrigTauTrigChain& chain);
  bool passedL2Trk(const TrigTauTrigChain& chain);
  bool passedEF(const TrigTauTrigChain& chain);

  void setParametrizedEFCuts( TrigTauEFSel& efsel,
                              CUTLEVEL level, float thresh);
  void setParametrizedEFCuts_201104( TrigTauEFSel& efsel,
                                     CUTLEVEL level, float thresh);
  
  TrigTauFlags::TRIGGERMENU usedTauMenu() const;
  CUTLEVEL                  usedCutLevel() const {return _cutLevel; }
  MENUVERSION               usedMenuVersion() const {return _menuVersion; }


  TrigTauL1Sel    l1Sel;
  TrigTauMETL1Sel l1METSel;

  TrigTauL2CalSel l2CalSel;
  TrigTauL2TrkSel l2TrkSel;

  TrigTauEFSel    efSel;

private:
  MENUVERSION _menuVersion;
  CUTLEVEL    _cutLevel;
  TrigTauFlags::TRIGGERMENU _usedType;
  TrigTauFlags::TRIGGERMENU _givenType;
  bool l2CalOnly;
  bool l2TrkOnly;

  void setTauMenu1206(TrigTauFlags::TRIGGERMENU type, CUTLEVEL level= DEFAULT );
  void setTauMenuJUNE07(TrigTauFlags::TRIGGERMENU type, CUTLEVEL level= DEFAULT );
  void setTauMenu13020 (TrigTauFlags::TRIGGERMENU type, CUTLEVEL level= DEFAULT );
  void setTauMenuAUG07 (TrigTauFlags::TRIGGERMENU type, CUTLEVEL level= DEFAULT );
  void setTauMenu13030pre (TrigTauFlags::TRIGGERMENU type, CUTLEVEL level= DEFAULT );
  void setTauMenu1400 (TrigTauFlags::TRIGGERMENU type, CUTLEVEL level= DEFAULT );
  void setTauMenu1520test (TrigTauFlags::TRIGGERMENU type, CUTLEVEL level= DEFAULT );
  void setTauMenu1560 (TrigTauFlags::TRIGGERMENU type, CUTLEVEL level= DEFAULT );
  void setTauMenu1600 (TrigTauFlags::TRIGGERMENU type, CUTLEVEL level= DEFAULT );
  void setTauMenu1662 (TrigTauFlags::TRIGGERMENU type, CUTLEVEL level= DEFAULT );
  void setTauMenu1666 (TrigTauFlags::TRIGGERMENU type, CUTLEVEL level= DEFAULT );
  void setTauMenuTest (TrigTauFlags::TRIGGERMENU type, CUTLEVEL level= DEFAULT );


};

#endif
