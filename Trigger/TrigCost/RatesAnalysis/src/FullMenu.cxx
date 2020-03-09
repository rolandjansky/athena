/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "FullMenu.h"

FullMenu::FullMenu( const std::string& name, ISvcLocator* pSvcLocator ) : RatesAnalysisAlg(name, pSvcLocator) {
}

FullMenu::~FullMenu() {
}

StatusCode  FullMenu::ratesInitialize() {
  ATH_MSG_INFO("In ratesInitialize()");

  // Here we assume a full-ring, other functions are available to change this assumption.
  // @see setTargetLumiMu(const double lumi, const double mu);
  // @see setTargetLumiBunches(const double lumi, const int32_t bunches);
  // @see setTargetMuBunches(const double mu, const int32_t bunches);
  setTargetLumi( m_lumi );

  // Register all triggers in the menu. Will not include any which were prescaled out in Athena.
  ATH_MSG_INFO("Adding all existing triggers");
  ATH_CHECK( addAllExisting() );

  // Or, if you were only interested in part of the menu you could do e.g.
  // ATH_CHECK( addExisting("HLT_mu.*") );
  // ATH_CHECK( addExisting("L1_MU.*") );

  return StatusCode::SUCCESS;
}

StatusCode  FullMenu::ratesExecute() {
  // Triggers added are of type kEXISTING so here we rely on the Trigger Decision Tool for pass/fail. 
  // This is automatic so there is nothing to do here.
  return StatusCode::SUCCESS;
}

StatusCode  FullMenu::ratesFinalize() {
  ATH_MSG_INFO("In ratesFinalize()");
  return StatusCode::SUCCESS;
}
