/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTAUMCTAU_H
#define TRIGTAUMCTAU_H
/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigTauMCTau
/// Package : TauTrigPerformance
/// Author  : Eric Torrence (University of Oregon)
/// Created : April 2006
///
/// DESCRIPTION:
///          MC truth for taus
///
///////////////////////////////////////////////////////////////////////////////////////////////////


#include <vector>
#include "TRef.h"
#include "TRefArray.h"
#include "TLorentzVector.h"
#include "TrigTauPerformNtuple/TrigTauTrueParticle.h"

class TrigTauMCTau : public TrigTauTrueParticle
{
 public:

  enum DECAY{ NONE, ENUNU, MUNUNU, RHONU, PINU, PR3, KNU, OTHERPR1, OTHER};

  TrigTauMCTau();
  TrigTauMCTau(const  TrigTauTrueParticle & other );
  virtual ~TrigTauMCTau();
  
  TrigTauMCTau &         operator = ( const TrigTauTrueParticle & other );
  TrigTauMCTau &         operator = ( const TrigTauMCTau & other );
  void print(int shift, bool completeTree) const;

  // TAU DECAY functions
  DECAY decayType() const;
  bool hadronicDecay() const;
  bool isOneProng() const;

  // Visible momentum
  const TLorentzVector& visible() const;

  // Leading track
  const TrigTauTrueParticle* leadingTrack() const;

 private :

  TLorentzVector _visible;
  void makeVisible();

 private:

  ClassDef(TrigTauMCTau, 1)  ;
};

#endif
