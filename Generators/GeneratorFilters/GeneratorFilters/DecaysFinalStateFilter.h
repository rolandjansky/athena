/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// GeneratorFilters/DecaysFinalStateFilter
//
// picks events with a given number of quarks, leptons and neutrinos from
// decays of a list of specified resonances (e.g. W, Z, ...)
//
// Authors:
// Kerim Suruliz Nov 2014
// Frank Siegert Nov 2014

#ifndef GENERATORFILTERSDECAYSFINALSTATEFILTER_H
#define GENERATORFILTERSDECAYSFINALSTATEFILTER_H

#include "GeneratorModules/GenFilter.h"

class DecaysFinalStateFilter : public GenFilter {

public:

  /// Constructor
  DecaysFinalStateFilter(const std::string& name, ISvcLocator* pSvcLocator);

  /// Destructor
  virtual ~DecaysFinalStateFilter() { }

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
