/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TTPVALVARIABLES_H_
#define _TTPVALVARIABLES_H_

#include "TTree.h"
#include "TChain.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TFile.h"
#include "TrigTauPerformNtuple/TrigTauMCTauList.h"
#include "TrigTauPerformNtuple/TrigTauFlags.h"
#include "TrigTauPerformNtuple/TrigTauTrack.h"
#include "TrigTauPerformAnalysis/TrigTauSelectionManager.h"
#include "TrigTauPerformAnalysis/TrigTauAnalyzer.h"
#include "TrigTauPerformAnalysis/TrigTauBugFix.h"
//#include "TrigTauPerformAnalysis/TrigTauL2TrkFEX.h"
#include <vector>
using namespace std;


class TTPValVariables {

private:
  char * filename;
  bool fileDefined;
  bool plotAllMatchedEvents;
  bool ignoreMatching;
  TChain * chain;
  bool debug;
  unsigned int nProng;
  TrigTauFlags::TRIGGERMENU menu;
  TrigTauFlags::HLTAUTHOR chainType;
  TrigTauSelectionManager::CUTLEVEL cutLevel;
  TrigTauMCTauList::OFFLINESELECTION offlineType;
  TrigTauSelectionManager::MENUVERSION menuVersion;
  bool signal;
  float maxVisEta, minLeadingPt;
  TrigTauAnalyzer analyzer;
  TrigTauSelectionManager selmanager;
  TrigTauBugFix bugfix;
  TrigTauEvent* event;
  unsigned int nL1Cuts;
  unsigned int nL2CaloCuts;
  unsigned int nL2TrkCuts;
  unsigned int nEFCuts;
  unsigned int nHistos;
  unsigned int nNames;

  void analyzeEvent(int eventNumber, bool chains, bool l1, bool l2 , bool efid, bool ef);
  void createHistograms(TH1F ** histos);
  void initialize(bool debug1 = false);
  
  struct less_Et : public binary_function<const TLorentzVector * ,const TLorentzVector * , bool> {
    bool operator()(const TLorentzVector *  x, const TLorentzVector  * y) { return x->Et() < y->Et(); }
  };
  // should do this template
  struct less_roiWordL1 : public binary_function<const TrigTauL1Object * ,const TrigTauL1Object  * , bool> {
    bool operator()(const TrigTauL1Object  *  x, const TrigTauL1Object  * y) { return x->m_roiWord < y->m_roiWord; }
  };
  struct less_roiWordEFID : public binary_function<const TrigTauTrackCollection * ,const TrigTauTrackCollection * , bool> {
    bool operator()(const TrigTauTrackCollection  *  x, const  TrigTauTrackCollection * y) { return x->m_roiWord < y->m_roiWord; }
  };

  struct less_EF : public binary_function<const TrigTauRecEFObject * ,const TrigTauRecEFObject * , bool> {
    bool operator()(const TrigTauRecEFObject *  x, const TrigTauRecEFObject * y) 
    { 
      if( x->m_roiWord != y->m_roiWord ) return x->m_roiWord < y->m_roiWord;
      if( x->m_seedType == y->m_seedType  ) return  x->Et() < y->Et(); 
      if(  x->m_seedType == 3 ) return false; // 3 is bigger
      if(  y->m_seedType == 3 ) return true;
      if(  x->m_seedType == 1 ) return false; // y can be only 2 then
      if(  y->m_seedType == 1 ) return true;   
      return false; // should never be here
    }
  };


public:

  TTPValVariables(TrigTauBugFix::TTPVERSION version = TrigTauBugFix::TTP15, bool debug1 = false);
  TTPValVariables ( char * filename1 , TrigTauBugFix::TTPVERSION version = TrigTauBugFix::TTP15,  bool debug1 = false);

  void analyze(int number = 10000000, bool chains=false, bool l1 = true, bool l2 = true, bool efid = true, bool ef = true);
   
  void setFile( char * filename1 );
  void setDebug(bool newValue) { debug = newValue; };
  void setMenu (TrigTauFlags::TRIGGERMENU newValue) { menu = newValue; };
  void setOfflineType (TrigTauMCTauList::OFFLINESELECTION newValue) {offlineType = newValue;};
  void setCutLevel ( TrigTauSelectionManager::CUTLEVEL newValue) { cutLevel = newValue; };
  void setMaxVisEta ( float newValue ) { maxVisEta = newValue; };
  void setMinLeadingPt (float newValue ) { minLeadingPt = newValue; };
  void setMenuVersion (TrigTauSelectionManager::MENUVERSION newValue ) { menuVersion = newValue;};
  void setPlotAllMatchedEvents (bool newValue) { plotAllMatchedEvents = newValue; };
  void setPlotAllEvents (bool newValue);
  void setSignal (bool newValue) {signal = newValue; };
  void setChainType (TrigTauFlags::HLTAUTHOR newValue) { chainType = newValue; };

  bool getDebug () { return debug; };
  TrigTauFlags::TRIGGERMENU getMenu () { return menu; };
  TrigTauMCTauList::OFFLINESELECTION getOfflineType () { return offlineType; };
  TrigTauSelectionManager::CUTLEVEL getCutLevel () { return cutLevel; };
  float getMaxVisEta() { return maxVisEta; };
  float getMinLeadingPt() { return minLeadingPt; };
  TrigTauSelectionManager::MENUVERSION getMenuVersion() { return menuVersion; };
  bool getPlotAllMatchedEvents () { return plotAllMatchedEvents; };
  bool getIgnoreMatching () { return ignoreMatching; };
  bool getSignal () { return signal; };
  TrigTauFlags::HLTAUTHOR getChainType() { return chainType;};



  int printL1(const TrigTauL1Object * l1object);
  int printL2Calo(const TrigTauL2Object * l2object);
  int printL2Trk(const TrigTauL2Object * l2object);
  int printL2Combined(const TrigTauL2Object * l2object);
  int printEF(const TrigTauRecEFObject * efobject);
  int printEFID(const TrigTauTrackCollection * object);

};


#endif
