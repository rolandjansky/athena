/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "RatesEmulationExample.h"

#include <xAODEgamma/ElectronContainer.h>

RatesEmulationExample::RatesEmulationExample( const std::string& name, ISvcLocator* pSvcLocator ) : RatesAnalysisAlg(name, pSvcLocator) {
}

RatesEmulationExample::~RatesEmulationExample() {
}

StatusCode RatesEmulationExample::ratesInitialize() {
  ATH_MSG_DEBUG("In ratesInitialize()");
  
  // Here we assume a full-ring, other functions are available to change this assumption.
  // @see setTargetLumiMu(const double lumi, const double mu);
  // @see setTargetLumiBunches(const double lumi, const int32_t bunches);
  // @see setTargetMuBunches(const double mu, const int32_t bunches);
  setTargetLumi( m_lumi );

  // Define triggers to emulate
  // TDT can be used instead by ATH_CHECK(addAllExisting());

  // name, prescale, expressPrescale, seedName, seedPrescale, groups
  std::set<std::string> triggerGroup {"RATE_SingleElectron"};
  ATH_CHECK(newTrigger("OFF_E10", 1, -1, "", 1, triggerGroup));

  // name, thresholdMin, thresholdMax
  ATH_CHECK(newScanTrigger("OFF_Ex", 20, 40));

  return StatusCode::SUCCESS;
}

StatusCode RatesEmulationExample::ratesExecute() {

  // Set decisions for kManual triggers
  const xAOD::ElectronContainer* electrons {nullptr};
  ATH_CHECK( evtStore()->retrieve(electrons, "Electrons") );
  std::set<double> electronpTs;
  for (const auto& e : *electrons) electronpTs.insert(e->pt()/1000.);
  if (electronpTs.size() >= 1 && *electronpTs.rbegin() >= 10.) ATH_CHECK(setTriggerDesicison("OFF_E10", true ));
  if (electronpTs.size() >= 1) ATH_CHECK(setTriggerDesicison("OFF_Ex", *electronpTs.rbegin() ));

  return StatusCode::SUCCESS;
}

StatusCode RatesEmulationExample::ratesFinalize() {
  ATH_MSG_DEBUG("In ratesFinalize()");
  return StatusCode::SUCCESS;
}
