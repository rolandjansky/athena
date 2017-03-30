/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ExampleRatesEmulation.h"

#include "xAODJet/Jet.h"
// #include "xAODMissingET/MissingET.h"
// #include "xAODMissingET/MissingETContainer.h"
#include "xAODMuon/Muon.h"
#include "xAODTau/TauJet.h"
#include "xAODEgamma/Photon.h"


ExampleRatesEmulation::ExampleRatesEmulation( const std::string& name, ISvcLocator* pSvcLocator ) : RatesAnalysisAlg(name, pSvcLocator) {
  declareProperty( "TargetLuminosity", m_lumi = 1e34);
}

ExampleRatesEmulation::~ExampleRatesEmulation() {
}

StatusCode  ExampleRatesEmulation::ratesInitialize() {
  ATH_MSG_INFO("In ratesInitialize()");

  // Here we assume a full-ring, other functions are available to change this assumption.
  // @see setTargetLumiMu(const double lumi, const double mu);
  // @see setTargetLumiBunches(const double lumi, const int32_t bunches);
  // setTargetLumi( m_lumi );

  const std::vector<double> jetBinEdges = {0,2.5,5,7.5,10,12.5,15,17.5,20,22.5,25,30,35,40,45,50,60,70,80,90,100,110,120,130,140,150,200,250,300,350,400,500};
  const std::vector<double> htBinEdges = {100,200,300,400,500,600,700,800,900,1000,1500,2000,2500};

  ATH_CHECK(newScanTrigger("OFF_Jx",  jetBinEdges, RatesScanTrigger::TriggerBehaviour_t::kTriggerBelowThreshold));
  ATH_CHECK(newScanTrigger("OFF_FATJx",  jetBinEdges, RatesScanTrigger::TriggerBehaviour_t::kTriggerBelowThreshold));
  ATH_CHECK(newScanTrigger("OFF_Jx.32ETA49",  jetBinEdges, RatesScanTrigger::TriggerBehaviour_t::kTriggerBelowThreshold));
  ATH_CHECK(newScanTrigger("OFF_4Jx", jetBinEdges, RatesScanTrigger::TriggerBehaviour_t::kTriggerBelowThreshold));

  ATH_CHECK(newScanTrigger("OFF_HTx", htBinEdges, RatesScanTrigger::TriggerBehaviour_t::kTriggerBelowThreshold));
  ATH_CHECK(newScanTrigger("OFF_XEx", jetBinEdges, RatesScanTrigger::TriggerBehaviour_t::kTriggerBelowThreshold));

  ATH_CHECK(newScanTrigger("OFF_MUx",  jetBinEdges, RatesScanTrigger::TriggerBehaviour_t::kTriggerBelowThreshold));
  ATH_CHECK(newScanTrigger("OFF_2MUx",  jetBinEdges, RatesScanTrigger::TriggerBehaviour_t::kTriggerBelowThreshold));

  ATH_CHECK(newScanTrigger("OFF_Gx",  jetBinEdges, RatesScanTrigger::TriggerBehaviour_t::kTriggerBelowThreshold));
  ATH_CHECK(newScanTrigger("OFF_2Gx",  jetBinEdges, RatesScanTrigger::TriggerBehaviour_t::kTriggerBelowThreshold));

  ATH_CHECK(newScanTrigger("OFF_TAUx",  jetBinEdges, RatesScanTrigger::TriggerBehaviour_t::kTriggerBelowThreshold));
  ATH_CHECK(newScanTrigger("OFF_2TAUx",  jetBinEdges, RatesScanTrigger::TriggerBehaviour_t::kTriggerBelowThreshold));

  ATH_CHECK(newScanTrigger("OFF_TAUx",  jetBinEdges, RatesScanTrigger::TriggerBehaviour_t::kTriggerBelowThreshold));
  ATH_CHECK(newScanTrigger("OFF_2TAUx",  jetBinEdges, RatesScanTrigger::TriggerBehaviour_t::kTriggerBelowThreshold));


  ATH_CHECK(newTrigger("OFF_MU35", 1, -1, "", 1, "RATE_Muon"));
  ATH_CHECK(newTrigger("OFF_2MU20", 1, -1, "", 1, "RATE_Muon"));

  ATH_CHECK(newTrigger("OFF_G40", 1, -1, "", 1, "RATE_EM"));
  ATH_CHECK(newTrigger("OFF_2G25", 1, -1, "", 1, "RATE_EM"));

  ATH_CHECK(newTrigger("OFF_G15_MU15", 1, -1, "", 1, "RATE_Combined"));

  ATH_CHECK(newTrigger("OFF_TAU130", 1, -1, "", 1, "RATE_Tau"));
  ATH_CHECK(newTrigger("OFF_2TAU50", 1, -1, "", 1, "RATE_Tau"));

  ATH_CHECK(newTrigger("OFF_J180", 1, -1, "", 1, "RATE_Jet"));
  ATH_CHECK(newTrigger("OFF_FATJ375", 1, -1, "", 1, "RATE_Jet"));
  ATH_CHECK(newTrigger("OFF_J80.32ETA49", 1, -1, "", 1, "RATE_Jet"));
  ATH_CHECK(newTrigger("OFF_4J50", 1, -1, "", 1, "RATE_Jet"));

  // ATH_CHECK(newTrigger("OFF_HT300", 1, 1, "", 1, "Rate_HTMET"));
  // ATH_CHECK(newTrigger("OFF_XE125", 1, 1, "", 1, "Rate_HTMET"));

  // ATH_CHECK(newTrigger("OFF_J100_XE100", 1, 1, "", 1, "RATE_Combined"));


  // ATH_CHECK(newTrigger("OFF_All", 1, 1, "", 1, ""));

  return StatusCode::SUCCESS;
}

StatusCode  ExampleRatesEmulation::ratesExecute() {

  const xAOD::JetContainer* jets = nullptr;
  ATH_CHECK( evtStore()->retrieve(jets, "AntiKt4LCTopoJets") );
  std::set<double> jetpTs; // Forces weak ordering
  std::set<double> forwardJetpTs;
  for (const auto& jet : *jets) {
    jetpTs.insert(jet->pt()/1000.);
    if (fabs(jet->eta()) > 3.2) forwardJetpTs.insert(jet->pt()/1000.);
  }

  double ht = 0.;
  const xAOD::JetContainer* fatjets = nullptr;
  ATH_CHECK( evtStore()->retrieve(fatjets, "CamKt12LCTopoJets") );
  std::set<double> fatJetpTs; // Forces weak ordering
  for (const auto& jet : *fatjets) {
    ht += jet->pt()/1000.;
    fatJetpTs.insert(jet->pt()/1000.);
  }

  const xAOD::TauJetContainer* tauJets = nullptr;
  ATH_CHECK( evtStore()->retrieve(tauJets, "TauJets") );
  std::set<double> tauJetpTs; // Forces weak ordering
  for (const auto& tau : *tauJets) {
    tauJetpTs.insert(tau->pt()/1000.);
  }

  const xAOD::MuonContainer* muons = nullptr;
  ATH_CHECK( evtStore()->retrieve(muons, "Muons") );
  std::set<double> muonpTs; // Forces weak ordering
  for (const auto& mu : *muons) muonpTs.insert(mu->pt()/1000.);

  const xAOD::PhotonContainer* photons = nullptr;
  ATH_CHECK( evtStore()->retrieve(photons, "Photons") );
  std::set<double> photonpTs; // Forces weak ordering
  for (const auto& g : *photons) photonpTs.insert(g->pt()/1000.);

  // const xAOD::MissingET* metObj = nullptr; 
  // ATH_CHECK( evtStore()->retrieve( metObj, "MET_LocHadTopo" ) );
  // const double met = metObj->met() / 1000.;

  // ATH_CHECK(setTriggerDesicison("OFF_All", true));

  //////////////

  if (jetpTs.size() >= 1) ATH_CHECK(setTriggerDesicison("OFF_Jx", *jetpTs.rbegin() ));
  if (jetpTs.size() >= 4) ATH_CHECK(setTriggerDesicison("OFF_4Jx",  *std::next(jetpTs.rbegin(), 3) ));
  if (fatJetpTs.size() >= 1) ATH_CHECK(setTriggerDesicison("OFF_FATJx",  *fatJetpTs.rbegin() ));
  if (forwardJetpTs.size() >= 1) ATH_CHECK(setTriggerDesicison("OFF_Jx.32ETA49",  *forwardJetpTs.rbegin() ));

  ATH_CHECK(setTriggerDesicison("OFF_HTx", ht ));
  // ATH_CHECK(setTriggerDesicison("OFF_XEx", met ));

  if (muonpTs.size() >= 1) ATH_CHECK(setTriggerDesicison("OFF_MUx", *muonpTs.rbegin() ));
  if (muonpTs.size() >= 2) ATH_CHECK(setTriggerDesicison("OFF_2MUx", *std::next(muonpTs.rbegin(), 1) ));

  if (tauJetpTs.size() >= 1) ATH_CHECK(setTriggerDesicison("OFF_TAUx", *tauJetpTs.rbegin() ));
  if (tauJetpTs.size() >= 2) ATH_CHECK(setTriggerDesicison("OFF_2TAUx", *std::next(tauJetpTs.rbegin(), 1) ));

  if (photonpTs.size() >= 1) ATH_CHECK(setTriggerDesicison("OFF_Gx", *photonpTs.rbegin() ));
  if (photonpTs.size() >= 2) ATH_CHECK(setTriggerDesicison("OFF_2Gx", *std::next(photonpTs.rbegin(), 1) ));

  //////////////

  if (muonpTs.size() >= 1 && *muonpTs.rbegin() >= 35.) ATH_CHECK(setTriggerDesicison("OFF_MU35", true ));
  if (muonpTs.size() >= 2 && *std::next(muonpTs.rbegin(), 1) >= 20.) ATH_CHECK(setTriggerDesicison("OFF_2MU20", true ));

  if (tauJetpTs.size() >= 1 && *tauJetpTs.rbegin() >= 130.) ATH_CHECK(setTriggerDesicison("OFF_TAU130", true ));
  if (tauJetpTs.size() >= 2 && *std::next(tauJetpTs.rbegin(), 1) >= 50.) ATH_CHECK(setTriggerDesicison("OFF_2TAU50", true ));

  if (photonpTs.size() >= 1 && *photonpTs.rbegin() >= 40.) ATH_CHECK(setTriggerDesicison("OFF_G40", true ));
  if (photonpTs.size() >= 2 && *std::next(photonpTs.rbegin(), 1) >= 25.) ATH_CHECK(setTriggerDesicison("OFF_2G25", true ));

  if (photonpTs.size() >= 1 && *photonpTs.rbegin() >= 15. &&
      muonpTs.size() >= 1 && *muonpTs.rbegin() >= 15.) ATH_CHECK(setTriggerDesicison("OFF_G15_MU15", true ));

  // if (jetpTs.size() >= 1 && *jetpTs.rbegin() >= 100. && met >= 100.) ATH_CHECK(setTriggerDesicison("OFF_J100_XE100", true ));

  if (jetpTs.size() >= 1 && *jetpTs.rbegin() >= 180.) ATH_CHECK(setTriggerDesicison("OFF_J180", true ));
  if (jetpTs.size() >= 4 && *std::next(jetpTs.rbegin(), 3) >= 50.) ATH_CHECK(setTriggerDesicison("OFF_4J50", true ));
  if (fatJetpTs.size() >= 1 && *fatJetpTs.rbegin() >= 375.) ATH_CHECK(setTriggerDesicison("OFF_FATJ375", true ));
  if (forwardJetpTs.size() >= 1 && *forwardJetpTs.rbegin() >= 80.) ATH_CHECK(setTriggerDesicison("OFF_J80.32ETA49", true ));

  // if (met >= 125.) ATH_CHECK(setTriggerDesicison("OFF_XE125", true ));
  // if (ht >= 300.) ATH_CHECK(setTriggerDesicison("OFF_HT300", true ));

  return StatusCode::SUCCESS;
}

StatusCode  ExampleRatesEmulation::ratesFinalize() {
  ATH_MSG_INFO("In ratesFinalize()");
  return StatusCode::SUCCESS;
}
