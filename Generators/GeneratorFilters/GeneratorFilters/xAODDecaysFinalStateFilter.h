/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// GeneratorFilters/DecaysFinalStateFilter
//
// picks events with a given number of quarks, leptons and neutrinos from
// decays of a list of specified resonances (e.g. W, Z, ...)
//
// Authors:
// Kerim Suruliz Nov 2014
// Frank Siegert Nov 2014

#ifndef GENERATORFILTERSXAODDECAYSFINALSTATEFILTER_H
#define GENERATORFILTERSXAODDECAYSFINALSTATEFILTER_H

#include "GeneratorModules/GenFilter.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"

#include "xAODTruth/TruthEvent.h"
#include "xAODTruth/TruthEventContainer.h"

class xAODDecaysFinalStateFilter : public GenFilter {

public:

  /// Constructor
  xAODDecaysFinalStateFilter(const std::string& name, ISvcLocator* pSvcLocator);

  /// Destructor
  virtual ~xAODDecaysFinalStateFilter() { }

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

  // list of allowed resonances from which decay products are counted
  std::vector<int> m_PDGAllowedParents;

  // required (exact) number of quarks, charged leptons, neutrinos and photons from decays
  int m_NQuarks, m_NChargedLeptons, m_NNeutrinos, m_NPhotons;

  // required minimal number of quarks, charged leptons, neutrinos and photons from decays
  int m_MinNQuarks, m_MinNChargedLeptons, m_MinNNeutrinos, m_MinNPhotons;

};

#endif
