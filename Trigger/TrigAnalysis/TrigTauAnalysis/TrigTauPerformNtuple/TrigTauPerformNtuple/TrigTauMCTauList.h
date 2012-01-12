/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTAUMCTAULIST_H
#define TRIGTAUMCTAULIST_H
/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigTauMCTauList
/// Package : TauTrigPerformance
/// Author  : Eric Torrence (University of Oregon)
/// Created : February 2007
///
/// DESCRIPTION:
///          List of true MC tau events
///          The pure tau list is created by removing duplicate tau records.
///          The good tau list has additional fiducial cuts defined
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <vector>
#include "TrigTauPerformNtuple/TrigTauFlags.h"
#include "TrigTauPerformNtuple/TrigTauMCEvent.h"
#include "TrigTauPerformNtuple/TrigTauMCTau.h"

class TrigTauEvent;

class TrigTauMCTauList
{
public:
  TrigTauMCTauList(const TrigTauMCEvent* mcEvent=0);
  virtual ~TrigTauMCTauList();

  // Fills mcTauPure and mcTauGood lists
  // Must be called in order
  void fillPureList(const TrigTauMCTauList & otherList);
  void fillPureList(const TrigTauMCEvent* mcEvent);
  void fillGoodList(); // Apply fiducial cuts, but no offline selection is done, 

  void printPureList() const;
  void printSettings() const;

  // Accessor functions
  unsigned int nMCTauPure() const; 
  unsigned int nMCTauGood() const; 
  const TrigTauMCTau & mcTauPureEntry(unsigned int i ) const;
  const TrigTauMCTau & mcTauGoodEntry(unsigned int i ) const;

  // Set MC truth fiducial cuts
  static float nominalMinVisPt(TrigTauFlags::TRIGGERMENU menu);
  void setMinVisPt(TrigTauFlags::TRIGGERMENU menu);
  void setMinVisPt(double minVisPt) {_minVisPt = minVisPt;} // In GeV // double here is to differentiate with setMinVisPt(TrigTauFlags::TRIGGERMENU menu)
  void setMaxVisEta(float maxVisEta) {_maxVisEta = maxVisEta;}
  void setMinLeadingPt(float minLeadingPt) {_minLeadPt = minLeadingPt; }; 

  // General decay mode fiducial cuts
  // All of these which are enabled are applied in order
  // By default, only selectHadronic requirement is applied
  void selectHadronic(bool sel=true) {_selHad = sel;} 

  void select1Prong(bool sel=true) {_sel1pr = sel;}  // Select only 1-prong
  void select3Prong(bool sel=true) {_sel3pr = sel;}  // Select 3 (or more)

  // Select specific decay mode (select NONE to disable)
  void selectDecayMode(TrigTauMCTau::DECAY decay) {_selMode = decay;}

  // Select offline reco match (removes non-matched from Good list)
  enum OFFLINESELECTION { TAU1P3P, TAUREC, EITHERTAU, BOTHTAU, TAUCUTSAFELOOSE, TAUCUTSAFEMEDIUM, TAUCUTSAFETIGHT, TAULLHLOOSE, TAULLHMEDIUM, TAULLHTIGHT, NONE, SAFELLHLOOSE, SAFELLHMEDIUM, SAFELLHTIGHT};
  enum OFFLINECRITERIA{ OFFLINEREL13, OFFLINEREL14};

  void setRecoSelection(OFFLINESELECTION selection, OFFLINECRITERIA criteria=OFFLINEREL14) { _offlineSelection = selection; 
    _offlineCriteria = criteria;};
  void setRecoMatchdR(float dR) {_recoMatchdR = dR;}
  
  void requireRecoMatch(const TrigTauEvent& event);
  void requireRecoMatch(const TrigTauEvent& event, OFFLINESELECTION selection, OFFLINECRITERIA criteria=OFFLINEREL14);

  // Access functions to return highest visible pt hadronic tau
  const TrigTauMCTau* leadingPtGoodTau() const;
  const TrigTauMCTau* nextPtGoodTau() const;


  // ---------------- apply fiducial cuts ----------------------
  bool isGoodTau(TrigTauMCTau& tau) const;

private:

  // Local storage of pure (non-duplicated) and good (fiducial cuts) taus
  std::vector<TrigTauMCTau>  _mcTauPure;  // Duplicates removed
  std::vector<TrigTauMCTau*> _mcTauGood;  // Fiducial cuts applied

  // Fiducial cuts
  bool _selHad;
  bool _sel1pr;
  bool _sel3pr;
  TrigTauMCTau::DECAY _selMode;

  float _minVisPt; // In GeV
  float _maxVisEta;
  float _minLeadPt;

  float _recoMatchdR;
  OFFLINESELECTION _offlineSelection;
  OFFLINECRITERIA  _offlineCriteria;
  

private:
  ClassDef(TrigTauMCTauList, 4) ;
};
#endif
