/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TRIGTAUMCEVENT_H_
#define  _TRIGTAUMCEVENT_H_
/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigTauMCEvent.h
/// Package : TrigTauPerformNtuple
/// Author  : Olya Igonkina
/// Created : December 2006
///
/// DESCRIPTION:
///
/// This class contains the vector of TrigTauTrueParticle
///     first N_0 particles from HEPEVT are saved
///     then the direct decay products of the particle with tau in the decay chain is saved
///     E.g t->Wb, W->tau nu_tau will have t, W, b, tau and nu_tau saved, but not decay products of b
///     all stable (e,mu,nu, pi,K,K_0, p, n) decay product of tau are saved as well, but no intermediate resonances
///
///     There is only 1 mother per daughter. If this is a problem, TrigTauTrueParticle class needs to be revised
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <vector>
#include "TObject.h"

#include "TrigTauPerformNtuple/TrigTauTrueParticle.h"
#include "TrigTauPerformNtuple/TrigTauEtMissObject.h"

class TrigTauMCEvent : public TObject
{
 public:

  TrigTauMCEvent();
  //TrigTauMCEvent(const TrigTauMCEvent & other);
  //operator
  virtual ~TrigTauMCEvent();
  virtual void reset();
  float recalculateEtMissTrue(); // overwrite original value with one based on neutrinos


  unsigned int nParticles()  const { return particleList.size(); };
  const  TrigTauTrueParticle & particleListEntry(unsigned int i ) const
  { return *particleList[i]; };

  unsigned int nTrueJets() const { return trueJets.size(); };
  const TLorentzVector & trueJetsEntry(unsigned int i ) const
  { return *trueJets[i]; };

  void print(const TrigTauTrueParticle & dau ) const;
  void print() const;
  
 public:    // ---------------- data members ------------------------  

  std::vector<TrigTauTrueParticle*>  particleList;
  TrigTauEtMissObject               etMissTrue;

  std::vector<TLorentzVector*>       trueJets;     

  float                             m_generatorWeight;

  ClassDef(TrigTauMCEvent, 4)  ;
    
};


#endif
