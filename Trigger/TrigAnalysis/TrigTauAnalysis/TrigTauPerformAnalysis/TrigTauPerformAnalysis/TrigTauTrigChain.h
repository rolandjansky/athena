/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTAUTRIGCHAIN_H
#define TRIGTAUTRIGCHAIN_H

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigTauTrigChain.h
/// Package : TrigTauPerformAnalysis
/// Author  : Eric Torrence (University of Oregon)
/// Created : February 2007
///
/// DESCRIPTION:
///           Container to associate L1-L2-EF into a trigger chain
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "TrigTauPerformNtuple/TrigTauL1Object.h"
#include "TrigTauPerformNtuple/TrigTauL2Object.h"
#include "TrigTauPerformNtuple/TrigTauRecEFObject.h"

#include "TrigTauPerformNtuple/TrigTauEtMissL1Object.h"

#include "TrigTauPerformNtuple/TrigTauRecOfflineObject.h"

class TrigTauTrigChain : public TObject {
 public:

  TrigTauTrigChain();
  virtual ~TrigTauTrigChain();
  
  TrigTauTrigChain& operator = (const TrigTauTrigChain& other );

  // The following function returns the 4-vector of the highest
  // available trigger object (EF, L2, L1) available in this chain.
  // This is intended for matching to the offline reco objects.

  const TLorentzVector* trigger() const;

  // public members
  const TrigTauL1Object*    L1;
  const TrigTauL2Object*    L2; 
  const TrigTauL2Object*    L2Cal; // if both L2Cal and L2Trk are not zero, 
  const TrigTauL2Object*    L2Trk; // they must point to the same object as L2
  const TrigTauRecEFObject* EF;

  const TrigTauEtMissL1Object* L1MET;

  const TrigTauRecOfflineObject* tauRec;

  void print() const;

 private:

  ClassDef(TrigTauTrigChain, 2) ;
};

#endif
