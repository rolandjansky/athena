/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// --------------------------------------------------
// 
// File:  GeneratorFilters/LeptonFilter.h
// Description: Filters based on presence of charged leptons
//
// Authors:
//         I Hinchliffe:  December 2001
//         A Buckley:     April 2009

#ifndef GENERATORFILTERS_XAODLEPTONFILTER_H
#define GENERATORFILTERS_XAODLEPTONFILTER_H

#include "GeneratorModules/GenFilter.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"

/// Filter events based on presence of charged leptons
class xAODLeptonFilter : public GenFilter {

public:

  /// Constructor
  xAODLeptonFilter(const std::string& name, ISvcLocator* pSvcLocator);

  /// Destructor
  virtual ~xAODLeptonFilter() { }

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

  // Declare filter variables
  Gaudi::Property<double> m_Ptmin{this,"Ptcut",10000.0,"Minimum pT for a lepton to count"};
  Gaudi::Property<double> m_Ptmax{this,"PtcutMax",1e99,"Maximum pT for a lepton to veto the event"};
  Gaudi::Property<double> m_EtaRange{this,"Etacut",10.0,"Minimum |pseudorapidity| for a lepton to count"};

};

#endif
