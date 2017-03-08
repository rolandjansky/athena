/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ExampleRatesEmulation.h"

#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"

ExampleRatesEmulation::ExampleRatesEmulation( const std::string& name, ISvcLocator* pSvcLocator ) : RatesAnalysisAlg(name, pSvcLocator) {
  declareProperty( "TargetLuminosity", m_lumi = 1e34);
}

namespace HelperFunctions {   
  struct pt_sort{
    inline bool operator() (const xAOD::IParticle& lhs, const xAOD::IParticle& rhs) { return (lhs.pt()  > rhs.pt());  }
    inline bool operator() (const xAOD::IParticle* lhs, const xAOD::IParticle* rhs) { return (lhs->pt() > rhs->pt()); }
  };
}

ExampleRatesEmulation::~ExampleRatesEmulation() {
}

StatusCode  ExampleRatesEmulation::ratesInitialize() {
  ATH_MSG_INFO("In ratesInitialize()");

  // Here we assume a full-ring, other functions are available to change this assumption.
  // @see setTargetLumiMu(const double lumi, const double mu);
  // @see setTargetLumiBunches(const double lumi, const int32_t bunches);
  setTargetLumi( m_lumi );

  const std::vector<double> jetBinEdges = {0,5,10,15,20,30,40,50,60,70};
  ATH_CHECK(newScanTrigger("L1_Jx",  jetBinEdges, RatesScanTrigger::TriggerBehaviour_t::kTriggerBelowThreshold));
  ATH_CHECK(newScanTrigger("L1_2Jx", jetBinEdges, RatesScanTrigger::TriggerBehaviour_t::kTriggerBelowThreshold));
  ATH_CHECK(newScanTrigger("L1_4Jx", jetBinEdges, RatesScanTrigger::TriggerBehaviour_t::kTriggerBelowThreshold));

  return StatusCode::SUCCESS;
}

StatusCode  ExampleRatesEmulation::ratesExecute() {

  const xAOD::JetContainer* jets = nullptr;
  ATH_CHECK( evtStore()->retrieve(jets, "AntiKt4LCTopoJets") );

  std::vector<double> jetpTs;
  for (const auto& jet : *jets) jetpTs.push_back(jet->pt()/1000.);
  std::sort(jetpTs.begin(), jetpTs.end(), std::greater<double>());

    ATH_MSG_DEBUG("Got " << jets->size() << " jets" );

  if (jets->size() < 1) return StatusCode::SUCCESS; // need 1 jet
  ATH_CHECK(setTriggerDesicison("L1_Jx", jetpTs.at(0)));

  if (jets->size() < 2) return StatusCode::SUCCESS; // need 2 jets
  ATH_CHECK(setTriggerDesicison("L1_2Jx", jetpTs.at(1)));

  if (jets->size() < 4) return StatusCode::SUCCESS; // need 4 jets
  ATH_CHECK(setTriggerDesicison("L1_4Jx", jetpTs.at(3)));

  return StatusCode::SUCCESS;
}

StatusCode  ExampleRatesEmulation::ratesFinalize() {
  ATH_MSG_INFO("In ratesFinalize()");
  return StatusCode::SUCCESS;
}
