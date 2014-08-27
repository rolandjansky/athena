/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// --------------------------------------------------
// 
// File:  GeneratorFilters/LeptonFilter.h
// Description: Filters based on presence of charged leptons
//
// Authors:
//         I Hinchliffe:  December 2001
//         A Buckley:     April 2009

#ifndef GENERATORFILTERSLEPTONFILTER_H
#define GENERATORFILTERSLEPTONFILTER_H

#include "GeneratorModules/GenFilter.h"


/// Filter events based on presence of charged leptons
class LeptonFilter : public GenFilter {

public:

  /// Constructor
  LeptonFilter(const std::string& name, ISvcLocator* pSvcLocator);

  /// Destructor
  virtual ~LeptonFilter() { }

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

  // Minimum pT for a lepton to count
  double m_Ptmin;

  // Maximum pT for a lepton to veto the event
  double m_Ptmax;

  // Minimum |pseudorapidity| for a lepton to count
  double m_EtaRange;

};

#endif
