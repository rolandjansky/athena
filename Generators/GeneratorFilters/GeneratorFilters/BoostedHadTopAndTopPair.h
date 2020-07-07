/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// --------------------------------------------------
// 
// File:  GeneratorFilters/BoostedHadTopAndTopPair.h
// Description: Filter for ttbar as function of the pT of the hadronic top
//
// Authors:
//         I Hinchliffe:  December 2001
//         A Buckley:     April 2009

#ifndef GeneratorFilter_BoostedHadTopAndTopPair_H
#define GeneratorFilter_BoostedHadTopAndTopPair_H

#include <iostream>
#include "GeneratorModules/GenFilter.h"

#include "AtlasHepMC/GenParticle_fwd.h"


/// Filter events based on presence of charged leptons
class BoostedHadTopAndTopPair : public GenFilter {

public:

  /// Constructor
  BoostedHadTopAndTopPair(const std::string& name, ISvcLocator* pSvcLocator);

  /// Destructor
  virtual ~BoostedHadTopAndTopPair() {
  }

  /// Initialize
  virtual StatusCode filterInitialize() {
    return StatusCode::SUCCESS;
  }

  /// Finalize
  virtual StatusCode filterFinalize() {
    return StatusCode::SUCCESS;
  }

  /// Do the filtering
  virtual StatusCode filterEvent();


private:

  // properties
  double m_tHadPtMin;
  double m_tHadPtMax;
  double m_tPairPtMin;  
  double m_tPairPtMax;
  int m_cutPtOf;

  HepMC::ConstGenParticlePtr  findInitial(HepMC::ConstGenParticlePtr part) const;
  bool  isFromTop(HepMC::ConstGenParticlePtr part) const;
  bool  isHadronic(HepMC::ConstGenParticlePtr part) const;
  bool  isFinalParticle(HepMC::ConstGenParticlePtr part) const;
  HepMC::FourVector momentumBofW(HepMC::ConstGenParticlePtr part);

};

#endif
