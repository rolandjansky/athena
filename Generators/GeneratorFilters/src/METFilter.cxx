/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METFilter: select events with true missing transverse energy above threshold
//
// Author: Robindra Prabhu, April 2006

#include "GeneratorFilters/METFilter.h"
#include "MissingETEvent/MissingET.h"
#include "MissingETEvent/MissingEtTruth.h"
#include "GaudiKernel/SystemOfUnits.h"


METFilter::METFilter(const std::string& name, ISvcLocator* pSvcLocator) :
  GenFilter(name, pSvcLocator)
{
  declareProperty("MissingEtCut", m_MissingEtCut = 20*Gaudi::Units::GeV);
  declareProperty("MissingEtUpperCut", m_MissingEtUpperCut = 1000000*Gaudi::Units::GeV);
  // Int=0, NonInt=1, IntFull=2, IntNonFull=3, IntOutCover=4 , Muons=5, Size=6, NotValid=999
  declareProperty("MissingEtCalcOption", m_MissingEtCalcOption = 1);
  declareProperty("MissingEtObjName",m_MissingEtObjName = "MET_Truth");
}


StatusCode METFilter::filterEvent() {
  ATH_MSG_DEBUG("METFilter FILTERING");

  // Retrieve MissingEtTruth container
  const MissingEtTruth* TruthMETContainer;
  CHECK(evtStore()->retrieve(TruthMETContainer, m_MissingEtObjName));
  if (!TruthMETContainer) {
    ATH_MSG_ERROR("No MissingEtTruth found in TDS");
    return StatusCode::SUCCESS;
  }

  // Calculate Missing Et
  double met_extrue = TruthMETContainer->exTruth(MissingEtTruth::TruthIndex(m_MissingEtCalcOption));
  double met_eytrue = TruthMETContainer->eyTruth(MissingEtTruth::TruthIndex(m_MissingEtCalcOption));
  double met_ettrue = sqrt(met_extrue*met_extrue + met_eytrue*met_eytrue);

  // Check if Missing Et is within filter threshold
  if ( (met_ettrue < m_MissingEtCut) || ( met_ettrue > m_MissingEtUpperCut) ) {
    setFilterPassed(false);
    return StatusCode::SUCCESS;
  }

  // Return if event passed
  ATH_MSG_INFO("Event Passed!, Missing ET= " << met_ettrue/Gaudi::Units::GeV);
  return StatusCode::SUCCESS;
}
