/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///===================================================
/// CutsSkim class
///---------------------------------------------------
/// Implements candidate selection for B2JpsiV0
/// candidates
///
/// Author: Daniel Scheirich <scheiric@cern.ch>
/// Part of B2JpsiV0RootAnalysis
///===================================================

#ifndef CutsSkim_H
#define CutsSkim_H

#include "CutsBase.h"
#include "GoodRunsLists/DQHelperFunctions.h"

// main class
class CutsSkim : public CutsBase {

  public:
    // costructor
    CutsSkim(OutputLevel olIn, std::string settings);

    // public methods
    bool selectTrigDecision(std::vector<std::string>* trigger,
                            std::string chain);                 // select event based on trigger decision information
    bool selectTrigPassThrough(std::vector<std::string>* pt,
                               std::string chain);              // select pass-through chains
    bool selectLevel1(std::vector<std::string>* trigger,
                      std::vector<std::string>* pt);            // select level-1 trigger decision
    bool selectTrigger(std::vector<std::string>* trigger1,
                       std::vector<std::string>* trigger2,
                       std::vector<std::string>* pt);           // select trigger decision (level-1 && level-2)


    bool selectGRL(int run, int lumiBlock);         // select event using GRL

    // cut variables
    // Trigger
    StringCut       cut_level1;             // apply level-1 trigger
    StringCut       cut_level2;             // apply level-2 trigger
    StringCut       cut_passThrough;        // pass-through chains, coma separated list. Only part of the name can be specified, e.g. _mu4_,_MU4_


    // Good Runs List
    StringCut       cut_GRL;                // good run list

    // list of pass-through chains
    std::vector<std::string>  passThrough;
    std::vector<std::string>  level1;
    std::vector<std::string>  level2;

};

///===================================================
/// Begin of definitions
///===================================================

CutsSkim::CutsSkim(OutputLevel olIn, std::string settings) : CutsBase(olIn, settings) {
  Log log("CutsSkim::CutsSkim", ol() );

  // initialize cut variables
  //trigger
  AddCutVariable("Level1"         , cut_level1        , "L1_MU0" );
  AddCutVariable("Level2"         , cut_level2        , "" );
  AddCutVariable("PassThrough"    , cut_passThrough   , "_MU4_,_mu4_" );

  log.info("Level1: ");
  level1 = explode(cut_level1.val, ",");
  std::vector<std::string>::iterator level1Itr = level1.begin();
  for(; level1Itr!=level1.end(); ++level1Itr) {
    log.info("  " + (*level1Itr));
  }

  log.info("Level2: ");
  level2 = explode(cut_level2.val, ",");
  std::vector<std::string>::iterator level2Itr = level2.begin();
  for(; level2Itr!=level2.end(); ++level2Itr) {
    log.info("  " + (*level2Itr));
  }

  log.info("Pass through: ");
  passThrough = explode(cut_passThrough.val, ",");
  std::vector<std::string>::iterator passThroughItr = passThrough.begin();
  for(; passThroughItr!=passThrough.end(); ++passThroughItr) {
    log.info("  " + (*passThroughItr));
  }

  //GRL
  AddCutVariable("GRL"            , cut_GRL           , ""       );
  if( cut_GRL.enabled ) {
    DQ::SetXMLFile(cut_GRL.val.c_str());
  }

}
///===================================================
bool CutsSkim::selectTrigDecision(std::vector<std::string>* trigger, std::string chain) {
  Log log("CutsSkim::selectLevel1", ol() );

  // loop through decisions
  std::vector<std::string>::iterator trigItr = trigger->begin();
  for(; trigItr!=trigger->end(); ++trigItr) {
    if( chain == (*trigItr) ) {
      return true;
    }
  }

  log.debug("Candidate not accepted by level-1 trigger");

  return false;

}
///===================================================
bool CutsSkim::selectTrigPassThrough(std::vector<std::string>* pt, std::string chain) {
  Log log("CutsSkim::selectTrigPassThrough", ol() );

  // loop through decisions
  std::vector<std::string>::iterator ptItr = pt->begin();
  for(; ptItr!=pt->end(); ++ptItr) {
    if( ptItr->find(chain) != std::string::npos) return true;
  }

  log.debug("Candidate not accepted by pass-through trigger");

  return false;

}
///===================================================
bool CutsSkim::selectLevel1(std::vector<std::string>* trigger, std::vector<std::string>* pt) {
  Log log("CutsSkim::selectLevel1", ol() );

  // level1
  if(cut_level1.enabled) {

    std::vector<std::string>::iterator level1Itr = level1.begin();
    for(; level1Itr!=level1.end(); ++level1Itr) {
      if( selectTrigDecision(trigger, *level1Itr) ) return true;
    }
  }

  // pass through chains
  if(cut_passThrough.enabled) {

    std::vector<std::string>::iterator passThroughItr = passThrough.begin();
    for(; passThroughItr!=passThrough.end(); ++passThroughItr) {
      if( selectTrigPassThrough(pt, *passThroughItr) ) return true;
    }
  }

  if(!cut_level1.enabled && !cut_passThrough.enabled) return true;

  return false;

}
///===================================================
bool CutsSkim::selectTrigger(std::vector<std::string>* trigger1, std::vector<std::string>* trigger2, std::vector<std::string>* pt) {
  Log log("CutsSkim::selectTrigger", ol() );

  if( !selectLevel1(trigger1, pt) ) return false;

  if(cut_level2.enabled) {

    std::vector<std::string>::iterator level2Itr = level2.begin();
    for(; level2Itr!=level2.end(); ++level2Itr) {
      if( selectTrigDecision(trigger2, *level2Itr) ) return true;
    }
  } else {
    return true;
  }

  return false;

}
///===================================================
bool CutsSkim::selectGRL(int run, int lumiBlock) {
  Log log("CutsSkim::selectGRL", ol() );

  if( cut_GRL.enabled && !DQ::PassRunLB(run,lumiBlock) ) return false;

  return true;

}
///===================================================

#endif

///===================================================
///      . .
///====o==V==o========================================
