/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigTauL2TrkSel.h
/// Package : TrigTauPerformAnalysis
/// Author  : Eric Torrence (University of Oregon)
/// Created : February 2007
///
/// DESCRIPTION:
///           Class to apply L2 selection to performance ntuple objects
///
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _TRIGTAUL2TRKSEL_H
#define _TRIGTAUL2TRKSEL_H

#include "TrigTauPerformNtuple/TrigTauFlags.h"
#include "TrigTauPerformAnalysis/TrigTauHistogram.h"

class TrigTauL2Object;

// Bits defining pattern returned by TrigTauL2TrkSel::cutWord()
const unsigned long L2CUT_NTRK      = 0x01;
const unsigned long L2CUT_NSLOW     = 0x02;
const unsigned long L2CUT_NISO      = 0x04;
const unsigned long L2CUT_NMATCHED  = 0x08;
const unsigned long L2CUT_CHARGE    = 0x10;
const unsigned long L2CUT_LEADPT    = 0x20;
const unsigned long L2CUT_RISO      = 0X40;
const unsigned long L2CUT_ETTRKEM   = 0x80;

//
// L2 track selector
//

class TrigTauL2TrkSel {
 public:
  TrigTauL2TrkSel();
  TrigTauL2TrkSel(const TrigTauL2TrkSel& other);

  virtual ~TrigTauL2TrkSel();

  void setCuts(int nTrMax, 
               int nTrISO, 
               int nTrSlow, 
               int nTrkMatched,
               float maxPtSlow = 6500, 
               float dRIso = 0.15, 
               float mPt = 1500, 
               float dRMatch = 0.15, 
               float dRt = 0.3,
	       float maxPtMin = 0,
	       bool  chargeCut = true,
	       float sumPtRatio = 100.,
	       float trkEmEt = 0.,
	       int nTrMin = 0);

  // Default is to use calo (eta,phi) to set core/isolation region
  //  void setCoreFromLeading(bool lead=true) {l2FEX.setCoreFromLeading(lead);}
  // Dummy routine to avoid compilation problems.  Doesn't do anything anymore!!!
  void setCoreFromLeading(bool) {return;}

  // Individual cut set functions
  void setCutNTracks(int nTrkMin, int nTrkMax)     
  {cut_NtracksMax = nTrkMax;cut_NtracksMin = nTrkMin; }
  void setCutNTrkIso(int nTrkIso)   {cut_NtracksIsolation = nTrkIso;}
  void setCutNTrkSlow(int nTrkSlow) {cut_NtracksSlow = nTrkSlow;}
  void setCutNTrkMatch(int nTrkMat) {cut_NtracksMatched = nTrkMat;}

  void setMinLeadPt(float leadPt) {maxPtMin = leadPt;}

  void setCutCharge(bool cut) {chargeCut = cut;}
  void setSumPtRatio(float ptr) {sumPtRatio = ptr;}
  void setCutTrkEmEt(float et) {trkEmEtMin = et;}

  // Use trigger decision bits instead
  void useBits(TrigTauFlags::TRIGGERMENU type) {menu=type;}

  void print() const;

  bool passed(const TrigTauL2Object& tau);

  // Return bits indicating cuts failed
  unsigned long cutWord(const TrigTauL2Object& tau);

  void setDebug(bool value=true) {_debug = value;}
  void setHistDebug(TrigTauHistogram::HISTTYPE type=TrigTauHistogram::PTHIST);

  TrigTauHistogram * histManager() { return m_histManager;};

  // Access to FEX information
  //  const TrigTauL2TrkFEX* getL2TrkFEX() const {return &l2FEX;}

 private:
  int       cut_NtracksMin;
  int       cut_NtracksMax;
  int       cut_NtracksIsolation;
  int       cut_NtracksSlow;
  int       cut_NtracksMatched;
  float     maxPtMin;
  bool      chargeCut;
  float     sumPtRatio;
  float     trkEmEtMin;

  bool _debug;

  // For bit-level selection
  TrigTauFlags::TRIGGERMENU menu;

  TrigTauHistogram * m_histManager;
};

#endif
