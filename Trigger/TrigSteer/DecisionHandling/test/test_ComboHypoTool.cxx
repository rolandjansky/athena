/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/WriteHandle.h"
#include "StoreGate/WriteHandleKey.h"
#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/EventContext.h"
#include "SGTools/TestStore.h"
#include "TestTools/initGaudi.h"
#include "TestTools/expect.h"
#include "TestTools/expect_exception.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "CxxUtils/checker_macros.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/ElectronAuxContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonAuxContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "DecisionHandling/DebugComboHypoTool.h"
#include "DecisionHandling/ComboHypo.h"

void test_set_pass(HLT::Identifier id, TrigCompositeUtils::Decision* d, Combo::LegDecisionsMap& ldm) {
  // Set the DecisionObject as having passed this leg.
  // In the real trigger, this happens in the chain's (or chain leg's) HypoTool, which runs in the HypoAlg prior to the ComboHypo
  TrigCompositeUtils::addDecisionID(id, d); 
  // Register the DecisionObject against the chain ID (or chain leg ID) 'id' in the Combo::LegDecisionsMap
  // In the real trigger, this happens in the ComboHypo and is the format used to communicate which decision objects are active on which legs
  // to the ComboHypo's ComboHypoTools (one per combined chain which needs to apply cuts which cover >1 legs, and/or >1 physics object per leg)
  ldm[id].push_back( TrigCompositeUtils::decisionToElementLink(d) );
}

/// @brief Test the ComboHypoTool class by simulating the execution of a variety of single- and multi-leg trigger topologies over 
/// and event with a total of 15 electrons, muons, jets of varying pT.
///
/// Tests the communication of active Decision Objects for various chains as communicated from the parent ComboHypo to the ComboHypoTool instances
/// Tests the use of the HLT::NestedUniqueCombinationGenerator to generate combinations over the objects for each chain's multiplicity requirement.
/// Tests the filtering of passed and failed Decision Objects based on the combined set of passing combinations.
///
/// The DebugComboHypoTool instance used for this test will accept any combination which has at least one physics object with pT > 45 MeV on each leg.
int main ATLAS_NOT_THREAD_SAFE () {

  using namespace TrigCompositeUtils;
  errorcheck::ReportMessage::hideFunctionNames (true);

  // initialize Gaudi, SG
  ISvcLocator* pSvcLoc;
  Athena_test::initGaudi(pSvcLoc); 
  StoreGateSvc* pSG(nullptr);
  assert( pSvcLoc->service("StoreGateSvc", pSG, true).isSuccess() );

  SmartIF<IMessageSvc> msgSvc{pSvcLoc};
  msgSvc->setOutputLevel(MSG::DEBUG);

  MsgStream log(msgSvc, "ComboHypoToolTest");

  // Create a context
  IProxyDict* xdict = &*pSG;
  xdict = pSG->hiveProxyDict();
  EventContext ctx(0,0);
  ctx.setExtension( Atlas::ExtendedEventContext(xdict) );
  Gaudi::Hive::setCurrentContext (ctx);
  log << MSG::INFO << "Context: " << ctx << endmsg;

  //
  // SETUP: Some test electrons, muons, jets. And a Decision Object to represent each in the trigger navigation
  //

  SG::WriteHandleKey<xAOD::ElectronContainer> electronContainerKey("My_ELEC_Container");
  SG::WriteHandleKey<xAOD::MuonContainer> muonContainerKey("My_MUON_Container");
  SG::WriteHandleKey<xAOD::JetContainer> jetContainerKey("My_JETS_Container");
  SG::WriteHandleKey<DecisionContainer> electronDecisionContainerKey("HLTNav_Elec");
  SG::WriteHandleKey<DecisionContainer> muonDecisionContainerKey("HLTNav_Muon");
  SG::WriteHandleKey<DecisionContainer> jetDecisionContainerKey("HLTNav_Jets");

  VALUE ( electronContainerKey.initialize() ) EXPECTED ( StatusCode::SUCCESS );
  VALUE ( muonContainerKey.initialize() ) EXPECTED ( StatusCode::SUCCESS );
  VALUE ( jetContainerKey.initialize() ) EXPECTED ( StatusCode::SUCCESS );
  VALUE ( electronDecisionContainerKey.initialize() ) EXPECTED ( StatusCode::SUCCESS );
  VALUE ( muonDecisionContainerKey.initialize() ) EXPECTED ( StatusCode::SUCCESS );
  VALUE ( jetDecisionContainerKey.initialize() ) EXPECTED ( StatusCode::SUCCESS );

  SG::WriteHandle<xAOD::ElectronContainer> electronContainer = createAndStoreWithAux<xAOD::ElectronContainer, xAOD::ElectronAuxContainer>(electronContainerKey, ctx);
  SG::WriteHandle<xAOD::MuonContainer> muonContainer = createAndStoreWithAux<xAOD::MuonContainer, xAOD::MuonAuxContainer>(muonContainerKey, ctx);
  SG::WriteHandle<xAOD::JetContainer> jetContainer = createAndStoreWithAux<xAOD::JetContainer, xAOD::MuonAuxContainer>(jetContainerKey, ctx);
  SG::WriteHandle<DecisionContainer> electronDecisionContainer = createAndStore(electronDecisionContainerKey, ctx);
  SG::WriteHandle<DecisionContainer> muonDecisionContainer = createAndStore(muonDecisionContainerKey, ctx);
  SG::WriteHandle<DecisionContainer> jetDecisionContainer = createAndStore(jetDecisionContainerKey, ctx);

  xAOD::ElectronContainer* electronContainerPtr = electronContainer.ptr();
  xAOD::MuonContainer* muonContainerPtr = muonContainer.ptr();
  xAOD::JetContainer* jetContainerPtr = jetContainer.ptr();
  DecisionContainer* electronDecisionContainerPtr = electronDecisionContainer.ptr();
  DecisionContainer* muonDecisionContainerPtr = muonDecisionContainer.ptr();
  DecisionContainer* jetDecisionContainerPtr = jetDecisionContainer.ptr();

  // Create 5 test electrons, muons, and jets. And their accompanying DecisionObjects, which will represent them in the navigation
  const size_t max_particles = 5;
  for (size_t i = 0; i < max_particles; ++i) {
    xAOD::Electron* e = new xAOD::Electron();
    electronContainerPtr->push_back(e);
    e->setP4(10.0*(max_particles-i), 0., 0., 0.);
    ElementLink<xAOD::ElectronContainer> el_e(*electronContainerPtr, electronContainerPtr->size()-1, ctx);

    Decision* d_e = new Decision();
    electronDecisionContainerPtr->push_back(d_e);
    d_e->setObjectLink<xAOD::ElectronContainer>(featureString(), el_e);

    xAOD::Muon* m = new xAOD::Muon();
    muonContainerPtr->push_back(m);
    m->setP4(10.0*(max_particles-i), 0., 0.);
    ElementLink<xAOD::MuonContainer> el_m(*muonContainerPtr, muonContainerPtr->size()-1, ctx);

    Decision* d_m = new Decision();
    muonDecisionContainerPtr->push_back(d_m);
    d_m->setObjectLink<xAOD::MuonContainer>(featureString(), el_m);

    xAOD::Jet* j = new xAOD::Jet();
    jetContainerPtr->push_back(j);
    j->setJetP4(xAOD::JetFourMom_t(10.0*(max_particles-i), 0., 0., 0.));
    ElementLink<xAOD::JetContainer> el_j(*jetContainerPtr, jetContainerPtr->size()-1, ctx);

    Decision* d_j = new Decision();
    jetDecisionContainerPtr->push_back(d_j);
    d_j->setObjectLink<xAOD::JetContainer>(featureString(), el_j);
  }

  //
  // SETUP: Parent Alg, and one Tool per test chain
  //

  std::vector<std::string> test_chains = {
    "HLT_e25", // Single object chain. Cannot think why anyone would schedule a ComboHypoTool for such a chain, test nonetheless.
    "HLT_e25_mu35", // Two-leg, asymmetric single objects chain
    "HLT_e35_e35", // Two-leg, symmetric single objects chain
    "HLT_2mu15", // One-leg, multi-object chain, required objects < available objects
    "HLT_5mu5", //  One-leg, multi-object chain, required objects == available objects
    "HLT_6mu5", //  One-leg, multi-object chain, required objects > available objects (!)
    "HLT_2e25_3mu15", // Two-leg, asymmetric multi-object chain
    "HLT_e35_2mu25_4j5", // Three-leg, asymmetric mixed multi-object and single-object chain
    "HLT_e35_2mu15_g25", // Three-leg, nothing supplied on 3rd leg (!)
    "HLT_2e35_e45_3mu15_mu35_j25" // Five-leg, mixed asymmetric and symmetric, mixed multi-object and single-object chain
  };

  Combo::MultiplicityReqMap mrm; // This is of type std::map<std::string, std::vector<int>>
  mrm["HLT_e25"] = {1};
  mrm["HLT_e25_mu35"] = {1,1};
  mrm["HLT_e35_e35"] = {1,1};
  mrm["HLT_2mu15"] = {2};
  mrm["HLT_5mu5"] = {5};
  mrm["HLT_6mu5"] = {6};
  mrm["HLT_2e25_3mu15"] = {2,3};
  mrm["HLT_e35_2mu25_4j5"] = {1,2,4};
  mrm["HLT_e35_2mu15_g25"] = {1,2,1};
  mrm["HLT_2e35_e45_3mu15_mu35_j25"] = {2,1,3,1,1};

  std::vector<DebugComboHypoTool*> test_tools;

  for (const std::string& test_chain : test_chains) {
    log << MSG::INFO << "Creating HypoTool for " << test_chain << endmsg;
    // NOTE: Pointers look to be owned by Gaudi. Don't delete these manually.
    ComboHypo* ch = new ComboHypo("ComboHypo", pSvcLoc);
    DebugComboHypoTool* dcht = new DebugComboHypoTool("DebugComboHypoTool", test_chain, ch);
    VALUE( dcht->initialize() ) EXPECTED ( StatusCode::SUCCESS );
    VALUE( dcht->setLegMultiplicity(mrm) ) EXPECTED ( StatusCode::SUCCESS );
    test_tools.push_back(dcht);
  }

  Combo::LegDecisionsMap passingLegs; // This is of type std::map<TrigCompositeUtils::DecisionID, ElementLinkVector<TrigCompositeUtils::DecisionContainer>>

  //
  // SETUP: HLT_e35. 3Choose1 = 3 combinations
  //
  HLT::Identifier HLT_e25 = HLT::Identifier("HLT_e25");

  // Set the result of the HLT_e25 HypoTool
  test_set_pass(HLT_e25, electronDecisionContainerPtr->at(0), passingLegs); // This is the 50 GeV electron
  test_set_pass(HLT_e25, electronDecisionContainerPtr->at(1), passingLegs); // This is the 40 GeV electron
  test_set_pass(HLT_e25, electronDecisionContainerPtr->at(2), passingLegs); // This is the 30 GeV electron

  //
  // SETUP: HLT_e25_mu35. 3Choose1 * 2Choose1 = 6 combinations
  //
  HLT::Identifier HLT_e25_mu35 = HLT::Identifier("HLT_e25_mu35");

  // Set the result of the leg000_HLT_e25_mu35 HypoTool
  test_set_pass(createLegName(HLT_e25_mu35, 0), electronDecisionContainerPtr->at(0), passingLegs); // This is the 50 GeV electron
  test_set_pass(createLegName(HLT_e25_mu35, 0), electronDecisionContainerPtr->at(1), passingLegs); // This is the 40 GeV electron
  test_set_pass(createLegName(HLT_e25_mu35, 0), electronDecisionContainerPtr->at(2), passingLegs); // This is the 30 GeV electron

  // Set the result of the leg001_HLT_e25_mu35 HypoTool 
  test_set_pass(createLegName(HLT_e25_mu35, 1), muonDecisionContainerPtr->at(0), passingLegs); // This is the 50 GeV muon
  test_set_pass(createLegName(HLT_e25_mu35, 1), muonDecisionContainerPtr->at(1), passingLegs); // This is the 40 GeV muon

  //
  // SETUP: HLT_e35_e35. 2Choose1 * 2Choose1 = 4 combinations
  //
  HLT::Identifier HLT_e35_e35 = HLT::Identifier("HLT_e35_e35");

  // Set the result of the leg000_HLT_e35_e35 HypoTool
  test_set_pass(createLegName(HLT_e35_e35, 0), electronDecisionContainerPtr->at(0), passingLegs); // This is the 50 GeV electron
  test_set_pass(createLegName(HLT_e35_e35, 0), electronDecisionContainerPtr->at(1), passingLegs); // This is the 40 GeV electron

  // Set the result of the leg001_HLT_e35_e35 HypoTool 
  test_set_pass(createLegName(HLT_e35_e35, 1), electronDecisionContainerPtr->at(0), passingLegs); // This is the 50 GeV electron
  test_set_pass(createLegName(HLT_e35_e35, 1), electronDecisionContainerPtr->at(1), passingLegs); // This is the 40 GeV electron

  //
  // SETUP: HLT_2mu15. 4Choose2 = 6 combinations
  //
  HLT::Identifier HLT_2mu15 = HLT::Identifier("HLT_2mu15");

  // Set the result of the HLT_2mu15 HypoTool 
  test_set_pass(HLT_2mu15, muonDecisionContainerPtr->at(0), passingLegs); // This is the 50 GeV muon
  test_set_pass(HLT_2mu15, muonDecisionContainerPtr->at(1), passingLegs); // This is the 40 GeV muon
  test_set_pass(HLT_2mu15, muonDecisionContainerPtr->at(2), passingLegs); // This is the 30 GeV muon
  test_set_pass(HLT_2mu15, muonDecisionContainerPtr->at(3), passingLegs); // This is the 20 GeV muon

  //
  // SETUP: HLT_5mu5. 5Choose5 = 1 combination
  //
  HLT::Identifier HLT_5mu5 = HLT::Identifier("HLT_5mu5");

  // Set the result of the HLT_5mu5 HypoTool 
  test_set_pass(HLT_5mu5, muonDecisionContainerPtr->at(0), passingLegs); // This is the 50 GeV muon
  test_set_pass(HLT_5mu5, muonDecisionContainerPtr->at(1), passingLegs); // This is the 40 GeV muon
  test_set_pass(HLT_5mu5, muonDecisionContainerPtr->at(2), passingLegs); // This is the 30 GeV muon
  test_set_pass(HLT_5mu5, muonDecisionContainerPtr->at(3), passingLegs); // This is the 20 GeV muon
  test_set_pass(HLT_5mu5, muonDecisionContainerPtr->at(4), passingLegs); // This is the 10 GeV muon

  //
  // SETUP: HLT_6mu5. 5Choose6. Not possible! Should exit before combination loop.
  //
  HLT::Identifier HLT_6mu5 = HLT::Identifier("HLT_6mu5");

  // Set the result of the HLT_6mu5 HypoTool 
  test_set_pass(HLT_6mu5, muonDecisionContainerPtr->at(0), passingLegs); // This is the 50 GeV muon
  test_set_pass(HLT_6mu5, muonDecisionContainerPtr->at(1), passingLegs); // This is the 40 GeV muon
  test_set_pass(HLT_6mu5, muonDecisionContainerPtr->at(2), passingLegs); // This is the 30 GeV muon
  test_set_pass(HLT_6mu5, muonDecisionContainerPtr->at(3), passingLegs); // This is the 20 GeV muon
  test_set_pass(HLT_6mu5, muonDecisionContainerPtr->at(4), passingLegs); // This is the 10 GeV muon

  //
  // SETUP: HLT_2e25_3mu15. 3Choose2 * 4Choose3 = 12 combinations
  //
  HLT::Identifier HLT_2e25_3mu15 = HLT::Identifier("HLT_2e25_3mu15");

  // Set the result of the leg000_HLT_2e25_3mu15 HypoTool
  test_set_pass(createLegName(HLT_2e25_3mu15, 0), electronDecisionContainerPtr->at(0), passingLegs); // This is the 50 GeV electron
  test_set_pass(createLegName(HLT_2e25_3mu15, 0), electronDecisionContainerPtr->at(1), passingLegs); // This is the 40 GeV electron
  test_set_pass(createLegName(HLT_2e25_3mu15, 0), electronDecisionContainerPtr->at(2), passingLegs); // This is the 30 GeV electron

  // Set the result of the leg001_HLT_2e25_3mu15 HypoTool 
  test_set_pass(createLegName(HLT_2e25_3mu15, 1), muonDecisionContainerPtr->at(0), passingLegs); // This is the 50 GeV muon
  test_set_pass(createLegName(HLT_2e25_3mu15, 1), muonDecisionContainerPtr->at(1), passingLegs); // This is the 40 GeV muon
  test_set_pass(createLegName(HLT_2e25_3mu15, 1), muonDecisionContainerPtr->at(2), passingLegs); // This is the 30 GeV muon
  test_set_pass(createLegName(HLT_2e25_3mu15, 1), muonDecisionContainerPtr->at(3), passingLegs); // This is the 20 GeV muon

  //
  // SETUP: HLT_e35_2mu25_4j5. 2Choose1 * 3Choose2 * 5Choose4 = 30 combinations
  //
  HLT::Identifier HLT_e35_2mu25_4j5 = HLT::Identifier("HLT_e35_2mu25_4j5");

  // Set the result of the leg000_HLT_e35_2mu25_4j5 HypoTool
  test_set_pass(createLegName(HLT_e35_2mu25_4j5, 0), electronDecisionContainerPtr->at(0), passingLegs); // This is the 50 GeV electron
  test_set_pass(createLegName(HLT_e35_2mu25_4j5, 0), electronDecisionContainerPtr->at(1), passingLegs); // This is the 40 GeV electron

  // Set the result of the leg001_HLT_e35_2mu25_4j5 HypoTool 
  test_set_pass(createLegName(HLT_e35_2mu25_4j5, 1), muonDecisionContainerPtr->at(0), passingLegs); // This is the 50 GeV muon
  test_set_pass(createLegName(HLT_e35_2mu25_4j5, 1), muonDecisionContainerPtr->at(1), passingLegs); // This is the 40 GeV muon
  test_set_pass(createLegName(HLT_e35_2mu25_4j5, 1), muonDecisionContainerPtr->at(2), passingLegs); // This is the 30 GeV muon

  // Set the result of the leg002_HLT_e35_2mu25_4j5 HypoTool 
  test_set_pass(createLegName(HLT_e35_2mu25_4j5, 2), jetDecisionContainerPtr->at(0), passingLegs); // This is the 50 GeV jet
  test_set_pass(createLegName(HLT_e35_2mu25_4j5, 2), jetDecisionContainerPtr->at(1), passingLegs); // This is the 40 GeV jet
  test_set_pass(createLegName(HLT_e35_2mu25_4j5, 2), jetDecisionContainerPtr->at(2), passingLegs); // This is the 30 GeV jet
  test_set_pass(createLegName(HLT_e35_2mu25_4j5, 2), jetDecisionContainerPtr->at(3), passingLegs); // This is the 20 GeV jet
  test_set_pass(createLegName(HLT_e35_2mu25_4j5, 2), jetDecisionContainerPtr->at(4), passingLegs); // This is the 10 GeV jet

  //
  // SETUP: HLT_e35_2mu15_g25. 2Choose1 * 4Choose2 * 0Choose1. Not possible! Should exit before combination loop.
  //
  HLT::Identifier HLT_e35_2mu15_g25 = HLT::Identifier("HLT_e35_2mu15_g25");

  // Set the result of the leg000_HLT_e35_2mu15_g25 HypoTool
  test_set_pass(createLegName(HLT_e35_2mu15_g25, 0), electronDecisionContainerPtr->at(0), passingLegs); // This is the 50 GeV electron
  test_set_pass(createLegName(HLT_e35_2mu15_g25, 0), electronDecisionContainerPtr->at(1), passingLegs); // This is the 40 GeV electron

  // Set the result of the leg001_HLT_e35_2mu15_g25 HypoTool 
  test_set_pass(createLegName(HLT_e35_2mu15_g25, 1), muonDecisionContainerPtr->at(0), passingLegs); // This is the 50 GeV muon
  test_set_pass(createLegName(HLT_e35_2mu15_g25, 1), muonDecisionContainerPtr->at(1), passingLegs); // This is the 40 GeV muon
  test_set_pass(createLegName(HLT_e35_2mu15_g25, 1), muonDecisionContainerPtr->at(2), passingLegs); // This is the 30 GeV muon
  test_set_pass(createLegName(HLT_e35_2mu15_g25, 1), muonDecisionContainerPtr->at(3), passingLegs); // This is the 20 GeV muon

  // Set the result of the leg002_HLT_e35_2mu15_g25 HypoTool 
  // - Nothing

  //
  // SETUP: HLT_2e35_e45_3mu15_mu35_j25. 2Choose2 * 1Choose1 * 4Choose3 * 2Choose1 * 3Choose1 = 1 * 1 * 4 * 2 * 3 = 24 combinations
  //
  HLT::Identifier HLT_2e35_e45_3mu15_mu35_j25 = HLT::Identifier("HLT_2e35_e45_3mu15_mu35_j25");

  // Set the result of the leg000_HLT_2e35_e45_3mu15_mu35_j25 HypoTool
  test_set_pass(createLegName(HLT_2e35_e45_3mu15_mu35_j25, 0), electronDecisionContainerPtr->at(0), passingLegs); // This is the 50 GeV electron
  test_set_pass(createLegName(HLT_2e35_e45_3mu15_mu35_j25, 0), electronDecisionContainerPtr->at(1), passingLegs); // This is the 40 GeV electron

  // Set the result of the leg001_HLT_2e35_e45_3mu15_mu35_j25 HypoTool 
  test_set_pass(createLegName(HLT_2e35_e45_3mu15_mu35_j25, 1), electronDecisionContainerPtr->at(0), passingLegs); // This is the 50 GeV electron

  // Set the result of the leg002_HLT_2e35_e45_3mu15_mu35_j25 HypoTool 
  test_set_pass(createLegName(HLT_2e35_e45_3mu15_mu35_j25, 2), muonDecisionContainerPtr->at(0), passingLegs); // This is the 50 GeV muon
  test_set_pass(createLegName(HLT_2e35_e45_3mu15_mu35_j25, 2), muonDecisionContainerPtr->at(1), passingLegs); // This is the 40 GeV muon
  test_set_pass(createLegName(HLT_2e35_e45_3mu15_mu35_j25, 2), muonDecisionContainerPtr->at(2), passingLegs); // This is the 30 GeV muon
  test_set_pass(createLegName(HLT_2e35_e45_3mu15_mu35_j25, 2), muonDecisionContainerPtr->at(3), passingLegs); // This is the 20 GeV muon

  // Set the result of the leg003_HLT_2e35_e45_3mu15_mu35_j25 HypoTool 
  test_set_pass(createLegName(HLT_2e35_e45_3mu15_mu35_j25, 3), muonDecisionContainerPtr->at(0), passingLegs); // This is the 50 GeV muon
  test_set_pass(createLegName(HLT_2e35_e45_3mu15_mu35_j25, 3), muonDecisionContainerPtr->at(1), passingLegs); // This is the 40 GeV muon

  // Set the result of the leg004_HLT_2e35_e45_3mu15_mu35_j25 HypoTool 
  test_set_pass(createLegName(HLT_2e35_e45_3mu15_mu35_j25, 4), jetDecisionContainerPtr->at(0), passingLegs); // This is the 50 GeV jet
  test_set_pass(createLegName(HLT_2e35_e45_3mu15_mu35_j25, 4), jetDecisionContainerPtr->at(1), passingLegs); // This is the 40 GeV jet
  test_set_pass(createLegName(HLT_2e35_e45_3mu15_mu35_j25, 4), jetDecisionContainerPtr->at(2), passingLegs); // This is the 30 GeV jet

  //
  // TEST: Each tool. Reference file based validation
  //

  log << MSG::INFO << "Running the HypoTools" << endmsg;

  for (DebugComboHypoTool* dcht : test_tools) {
    VALUE ( dcht->decide(passingLegs, ctx) ) EXPECTED ( StatusCode::SUCCESS );
  }

  return 0;
  
}
