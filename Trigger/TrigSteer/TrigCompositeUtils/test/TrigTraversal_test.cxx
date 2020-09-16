/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

template<class CONTAINER>
void printFeatures(const std::vector< TrigCompositeUtils::LinkInfo<CONTAINER> >& featureContainer, const std::string& name, MsgStream& log);

/// @brief Test to check traversal functions of a graph of interconnect TrigComposite objects
///
/// This test hard-codes a Run 3 navigation structure and tests that the correct 
///
int main ATLAS_NOT_THREAD_SAFE () {

  using namespace TrigCompositeUtils;
  xAOD::TrigComposite::s_throwOnCopyError = true;

  errorcheck::ReportMessage::hideFunctionNames (true);

  // initialize Gaudi, SG
  ISvcLocator* pSvcLoc;
  Athena_test::initGaudi(pSvcLoc); 
  StoreGateSvc* pSG(nullptr);
  assert( pSvcLoc->service("StoreGateSvc", pSG, true).isSuccess() );

  MsgStream log(nullptr, "TrigTraversal");

  // Create a context
  IProxyDict* xdict = &*pSG;
  xdict = pSG->hiveProxyDict();
  EventContext ctx(0,0);
  ctx.setExtension( Atlas::ExtendedEventContext(xdict) );
  Gaudi::Hive::setCurrentContext (ctx);
  log << "Context: " << ctx << endmsg;

  // check current context
  const EventContext& ctx1 = Gaudi::Hive::currentContext();
  log << "Current context: " << ctx1 << endmsg;

  SG::WriteHandleKey<DecisionContainer> decisionContainerKey("HLTNav_MyDecisionContainer");
  SG::WriteHandleKey<xAOD::ElectronContainer> electronContainerKey("MyElectronContainer");
  SG::WriteHandleKey<xAOD::MuonContainer> muonContainerKey("MyMuonContainer");

  decisionContainerKey.initialize().ignore();
  electronContainerKey.initialize().ignore();
  muonContainerKey.initialize().ignore();

  SG::WriteHandle<DecisionContainer> decisionContainer = createAndStore( decisionContainerKey);
  DecisionContainer* decisionContainerPtr = decisionContainer.ptr();

  SG::WriteHandle<xAOD::ElectronContainer> electronContainer = createAndStoreWithAux<xAOD::ElectronContainer, xAOD::ElectronAuxContainer>( electronContainerKey, ctx1 );
  xAOD::ElectronContainer* electronContainerPtr = electronContainer.ptr();

  SG::WriteHandle<xAOD::MuonContainer> muonContainer = createAndStoreWithAux<xAOD::MuonContainer, xAOD::MuonAuxContainer>( muonContainerKey, ctx1 );
  xAOD::MuonContainer* muonContainerPtr = muonContainer.ptr();

  // Create a sufficiency complicated tree structure by hand with known expected output 
  // In the following 
  //    F = Filter
  //    IM = Input Maker
  //    H = Hypo
  //    CH = CombinedHypo

  constexpr unsigned HLT_mufast_chain = 1;
  constexpr unsigned HLT_mu_chain = 2;
  constexpr unsigned HLT_mu_em_chain = 3;
  constexpr unsigned HLT_em_chain = 4;

  DecisionIDContainer mufast_IDcont {HLT_mufast_chain};
  DecisionIDContainer mu_IDcont {HLT_mu_chain};
  DecisionIDContainer mu_em_IDcont {HLT_mu_em_chain};
  DecisionIDContainer em_IDcont {HLT_em_chain};
  DecisionIDContainer all_IDcont {HLT_mufast_chain, HLT_mu_chain, HLT_mu_em_chain, HLT_em_chain};


  // Starting nodes
  Decision* MU0 = newDecisionIn(decisionContainerPtr, "MU0");
  Decision* MU1 = newDecisionIn(decisionContainerPtr, "MU1");
  Decision* EM0 = newDecisionIn(decisionContainerPtr, "EM0");

  // Terminus node
  Decision* END = newDecisionIn(decisionContainerPtr, "HLTPassRaw"); // This name is important
  ElementLink<DecisionContainer> end_link(*decisionContainerPtr, decisionContainerPtr->size() - 1, ctx1);

  // First muon ROI
  xAOD::Muon* rec_1__mu0 = new xAOD::Muon(); // Step1 muon
  muonContainerPtr->push_back(rec_1__mu0);
  rec_1__mu0->setP4(5., 0., 0.);
  ElementLink<xAOD::MuonContainer> rec_1__mu0_link(*muonContainerPtr, muonContainerPtr->size() - 1, ctx1);
  
  // No rec_2__mu0. Fails hypo
  //
  //

  // Second muon ROI
  xAOD::Muon* rec_1__mu1 = new xAOD::Muon(); // Step1 muon
  muonContainerPtr->push_back(rec_1__mu1);
  rec_1__mu1->setP4(20., 0., 0.);
  ElementLink<xAOD::MuonContainer> rec_1__mu1_link(*muonContainerPtr, muonContainerPtr->size() - 1, ctx1);

  xAOD::Muon* rec_2__mu1 = new xAOD::Muon(); // Step2 muon
  muonContainerPtr->push_back(rec_2__mu1);
  rec_2__mu1->setP4(21., 0, 0);
  ElementLink<xAOD::MuonContainer> rec_2__mu1_link(*muonContainerPtr, muonContainerPtr->size() - 1, ctx1);

  // First EM ROI
  xAOD::Electron* rec_1__em0 = new xAOD::Electron(); // Step1 electron
  electronContainerPtr->push_back(rec_1__em0);
  rec_1__em0->setP4(30., 0., 0., 0.);
  ElementLink<xAOD::ElectronContainer> rec_1__em0_link(*electronContainerPtr, electronContainerPtr->size() - 1, ctx1);

  xAOD::Electron* rec_2__em0 = new xAOD::Electron(); // Step2 electron
  electronContainerPtr->push_back(rec_2__em0);
  rec_2__em0->setP4(31., 0., 0., 0.);
  ElementLink<xAOD::ElectronContainer> rec_2__em0_link(*electronContainerPtr, electronContainerPtr->size() - 1, ctx1);

  ///
  /// Muon RoI 0, single muon chains ID:HLT_mufast_chain, ID:2. Fails both chains at first Hypo
  ///

  {
    addDecisionID(HLT_mufast_chain, MU0);
    addDecisionID(HLT_mu_chain, MU0);

    Decision* MU_F_1__MU0 = newDecisionIn(decisionContainerPtr, "MU_F_1__MU0");
    linkToPrevious(MU_F_1__MU0, MU0);
    addDecisionID(HLT_mufast_chain, MU_F_1__MU0);
    addDecisionID(HLT_mu_chain, MU_F_1__MU0);

    Decision* MU_IM_1__MU0 = newDecisionIn(decisionContainerPtr, "MU_IM_1__MU0");
    linkToPrevious(MU_IM_1__MU0, MU_F_1__MU0);
    addDecisionID(HLT_mufast_chain, MU_IM_1__MU0);
    addDecisionID(HLT_mu_chain, MU_IM_1__MU0);

    Decision* MU_H_1__MU0 = newDecisionIn(decisionContainerPtr, "MU_H_1__MU0");
    linkToPrevious(MU_H_1__MU0, MU_IM_1__MU0);
    MU_H_1__MU0->setObjectLink<xAOD::MuonContainer>(featureString(), rec_1__mu0_link);
    // Fails HLT_mufast_chain
    // Fails HLT_mu_chain

  }

  ///
  /// Muon RoI 0, combined muon+electron chain ID:3. Fails first hypo
  ///

  {
    addDecisionID(HLT_mu_em_chain, MU0);

    Decision* MUEM_F_1__MU0 = newDecisionIn(decisionContainerPtr, "MUEM_F_1__MU0");
    linkToPrevious(MUEM_F_1__MU0, MU0);
    addDecisionID(HLT_mu_em_chain, MUEM_F_1__MU0);

    Decision* MUEM_IM_1__MU0 = newDecisionIn(decisionContainerPtr, "MUEM_IM_1__MU0");
    linkToPrevious(MUEM_IM_1__MU0, MUEM_F_1__MU0);
    addDecisionID(HLT_mu_em_chain, MUEM_IM_1__MU0);

    Decision* MUEM_H_1__MU0 = newDecisionIn(decisionContainerPtr, "MUEM_H_1__MU0");
    linkToPrevious(MUEM_H_1__MU0, MUEM_IM_1__MU0);
    MUEM_H_1__MU0->setObjectLink<xAOD::MuonContainer>(featureString(), rec_1__mu0_link);
    // Fails HLT_mu_em_chain

    Decision* MUEM_CH_1__MU0 = newDecisionIn(decisionContainerPtr, "MUEM_CH_1__MU0");
    linkToPrevious(MUEM_CH_1__MU0, MUEM_H_1__MU0);
    // Note: Combo hypo does not re-link to feature.
    // Fails HLT_mu_em_chain

  }

  ///
  /// Muon RoI 1, single muon chain ID:HLT_mufast_chain passes event after first Hypo. Single muon chain ID:HLT_mu_chain passes event after second hypo
  ///

  {
    addDecisionID(HLT_mufast_chain, MU1);
    addDecisionID(HLT_mu_chain, MU1);

    Decision* MU_F_1__MU1 = newDecisionIn(decisionContainerPtr, "MU_F_1__MU1");
    linkToPrevious(MU_F_1__MU1, MU1);
    addDecisionID(HLT_mufast_chain, MU_F_1__MU1);
    addDecisionID(HLT_mu_chain, MU_F_1__MU1);

    Decision* MU_IM_1__MU1 = newDecisionIn(decisionContainerPtr, "MU_IM_1__MU1");
    linkToPrevious(MU_IM_1__MU1, MU_F_1__MU1);
    addDecisionID(HLT_mufast_chain, MU_IM_1__MU1);
    addDecisionID(HLT_mu_chain, MU_IM_1__MU1);

    Decision* MU_H_1__MU1 = newDecisionIn(decisionContainerPtr, "MU_H_1__MU1");
    linkToPrevious(MU_H_1__MU1, MU_IM_1__MU1);
    MU_H_1__MU1->setObjectLink<xAOD::MuonContainer>(featureString(), rec_1__mu1_link);
    addDecisionID(HLT_mufast_chain, MU_H_1__MU1);
    addDecisionID(HLT_mu_chain, MU_H_1__MU1);
    // HLT_mufast_chain passes the event
    addDecisionID(HLT_mufast_chain, END);
    linkToPrevious(END, MU_H_1__MU1);

    /// !!!
    /// !!!
    /// !!! When finding the first feature for the HLT_mu_chain, the navigation is here given two entry points into the graph,
    /// !!! the correct one (MU_H_2__MU1) and an incorrect one (MU_H_1__MU1) due to HLT_mufast_chain also passing this event
    /// !!! at this earlier Step.
    /// !!!
    /// !!! The feature access is currently "dumb", it doesn't know what should be the final step for each chain. Hence here
    /// !!! it will return both the Step1 muon and the Step2 muon when asked for the first feature down each leg for HLT_mu_chain,
    /// !!! Rather than just the Step2 muon.
    /// !!!
    /// !!! This will be corrected later, once the Trigger Decision Tool has access to more menu reflection information.
    /// !!!
    /// !!!

    Decision* MU_F_2__MU1 = newDecisionIn(decisionContainerPtr, "MU_F_2__MU1");
    linkToPrevious(MU_F_2__MU1, MU_H_1__MU1);
    addDecisionID(HLT_mu_chain, MU_F_2__MU1);

    Decision* MU_IM_2__MU1 = newDecisionIn(decisionContainerPtr, "MU_IM_2__MU1");
    linkToPrevious(MU_IM_2__MU1, MU_F_2__MU1);
    addDecisionID(HLT_mu_chain, MU_IM_2__MU1);

    Decision* MU_H_2__MU1 = newDecisionIn(decisionContainerPtr, "MU_H_2__MU1");
    linkToPrevious(MU_H_2__MU1, MU_IM_2__MU1);
    MU_H_2__MU1->setObjectLink<xAOD::MuonContainer>(featureString(), rec_2__mu1_link);
    addDecisionID(HLT_mu_chain, MU_H_2__MU1);
    // HLT_mu_chain passes the event
    addDecisionID(HLT_mu_chain, END);
    linkToPrevious(END, MU_H_2__MU1);
  }

  ///
  /// Muon RoI 1, combined muon+electron chain ID:HLT_mu_em_chain. Passes first and second hypo. Passes EM leg (later)
  ///

  {
    addDecisionID(HLT_mu_em_chain, MU1);

    Decision* MUEM_F_1__MU1 = newDecisionIn(decisionContainerPtr, "MUEM_F_1__MU1");
    linkToPrevious(MUEM_F_1__MU1, MU1);
    addDecisionID(HLT_mu_em_chain, MUEM_F_1__MU1);

    Decision* MUEM_IM_1__MU1 = newDecisionIn(decisionContainerPtr, "MUEM_IM_1__MU1");
    linkToPrevious(MUEM_IM_1__MU1, MUEM_F_1__MU1);
    addDecisionID(HLT_mu_em_chain, MUEM_IM_1__MU1);

    Decision* MUEM_H_1__MU1 = newDecisionIn(decisionContainerPtr, "MUEM_H_1__MU1");
    linkToPrevious(MUEM_H_1__MU1, MUEM_IM_1__MU1);
    MUEM_H_1__MU1->setObjectLink<xAOD::MuonContainer>(featureString(), rec_1__mu1_link);
    addDecisionID(HLT_mu_em_chain, MUEM_H_1__MU1);

    Decision* MUEM_CH_1__MU1 = newDecisionIn(decisionContainerPtr, "MUEM_CH_1__MU1");
    linkToPrevious(MUEM_CH_1__MU1, MUEM_H_1__MU1);
    addDecisionID(HLT_mu_em_chain, MUEM_CH_1__MU1);

    Decision* MUEM_F_2__MU1 = newDecisionIn(decisionContainerPtr, "MUEM_F_2__MU1");
    linkToPrevious(MUEM_F_2__MU1, MUEM_CH_1__MU1);
    addDecisionID(HLT_mu_em_chain, MUEM_F_2__MU1);

    Decision* MUEM_IM_2__MU1 = newDecisionIn(decisionContainerPtr, "MUEM_IM_2__MU1");
    linkToPrevious(MUEM_IM_2__MU1, MUEM_F_2__MU1);
    addDecisionID(HLT_mu_em_chain, MUEM_IM_2__MU1);

    Decision* MUEM_H_2__MU1 = newDecisionIn(decisionContainerPtr, "MUEM_H_2__MU1");
    linkToPrevious(MUEM_H_2__MU1, MUEM_IM_2__MU1);
    MUEM_H_2__MU1->setObjectLink<xAOD::MuonContainer>(featureString(), rec_2__mu1_link);
    addDecisionID(HLT_mu_em_chain, MUEM_H_2__MU1);

    Decision* MUEM_CH_2__MU1 = newDecisionIn(decisionContainerPtr, "MUEM_CH_2__MU1");
    linkToPrevious(MUEM_CH_2__MU1, MUEM_H_2__MU1);
    addDecisionID(HLT_mu_em_chain, MUEM_CH_2__MU1);
    // HLT_mu_em_chain passes the event
    addDecisionID(HLT_mu_em_chain, END);
    linkToPrevious(END, MUEM_CH_2__MU1);

  }

  ///
  /// EM RoI 0, single electron chain ID:4. Passes first and second hypo. Passes EM leg (later)
  ///

  {
    addDecisionID(HLT_em_chain, EM0);

    Decision* EM_F_1__EM0 = newDecisionIn(decisionContainerPtr, "EM_F_1__EM0");
    linkToPrevious(EM_F_1__EM0, EM0);
    addDecisionID(HLT_em_chain, EM_F_1__EM0);

    Decision* EM_IM_1__EM0 = newDecisionIn(decisionContainerPtr, "EM_IM_1__EM0");
    linkToPrevious(EM_IM_1__EM0, EM_F_1__EM0);
    addDecisionID(HLT_em_chain, EM_IM_1__EM0);

    Decision* EM_H_1__EM0 = newDecisionIn(decisionContainerPtr, "EM_H_1__EM0");
    linkToPrevious(EM_H_1__EM0, EM_IM_1__EM0);
    EM_H_1__EM0->setObjectLink<xAOD::ElectronContainer>(featureString(), rec_1__em0_link);
    addDecisionID(HLT_em_chain, EM_H_1__EM0);

    Decision* EM_F_2__EM0 = newDecisionIn(decisionContainerPtr, "EM_F_2__EM0");
    linkToPrevious(EM_F_2__EM0, EM_H_1__EM0);
    addDecisionID(HLT_em_chain, EM_F_2__EM0);

    Decision* EM_IM_2__EM0 = newDecisionIn(decisionContainerPtr, "EM_IM_2__EM0");
    linkToPrevious(EM_IM_2__EM0, EM_F_2__EM0);
    addDecisionID(HLT_em_chain, EM_IM_2__EM0);

    Decision* EM_H_2__EM0 = newDecisionIn(decisionContainerPtr, "EM_H_2__EM0");
    linkToPrevious(EM_H_2__EM0, EM_IM_2__EM0);
    EM_H_2__EM0->setObjectLink<xAOD::ElectronContainer>(featureString(), rec_2__em0_link);
    addDecisionID(HLT_em_chain, EM_H_2__EM0);
    // HLT_em_chain passes the event
    linkToPrevious(END, EM_H_2__EM0);
    addDecisionID(HLT_em_chain, END);
  }

  ///
  /// EM RoI 0, combined muon+electron chain ID:HLT_mu_em_chain. Passes first and second hypo. Passes MU leg (earlier)
  ///

  {
    addDecisionID(HLT_mu_em_chain, EM0);

    Decision* MUEM_F_1__EM0 = newDecisionIn(decisionContainerPtr, "MUEM_F_1__EM0");
    linkToPrevious(MUEM_F_1__EM0, EM0);
    addDecisionID(HLT_mu_em_chain, MUEM_F_1__EM0);

    Decision* MUEM_IM_1__EM0 = newDecisionIn(decisionContainerPtr, "MUEM_IM_1__EM0");
    linkToPrevious(MUEM_IM_1__EM0, MUEM_F_1__EM0);
    addDecisionID(HLT_mu_em_chain, MUEM_IM_1__EM0);

    Decision* MUEM_H_1__EM0 = newDecisionIn(decisionContainerPtr, "MUEM_H_1__EM0");
    linkToPrevious(MUEM_H_1__EM0, MUEM_IM_1__EM0);
    MUEM_H_1__EM0->setObjectLink<xAOD::ElectronContainer>(featureString(), rec_1__em0_link);
    addDecisionID(HLT_mu_em_chain, MUEM_H_1__EM0);

    Decision* MUEM_CH_1__EM0 = newDecisionIn(decisionContainerPtr, "MUEM_CH_1__EM0");
    linkToPrevious(MUEM_CH_1__EM0, MUEM_H_1__EM0);
    addDecisionID(HLT_mu_em_chain, MUEM_CH_1__EM0);

    Decision* MUEM_F_2__EM0 = newDecisionIn(decisionContainerPtr, "MUEM_F_2__EM0");
    linkToPrevious(MUEM_F_2__EM0, MUEM_CH_1__EM0);
    addDecisionID(HLT_mu_em_chain, MUEM_F_2__EM0);

    Decision* MUEM_IM_2__EM0 = newDecisionIn(decisionContainerPtr, "MUEM_IM_2__EM0");
    linkToPrevious(MUEM_IM_2__EM0, MUEM_F_2__EM0);
    addDecisionID(HLT_mu_em_chain, MUEM_IM_2__EM0);

    Decision* MUEM_H_2__EM0 = newDecisionIn(decisionContainerPtr, "MUEM_H_2__EM0");
    linkToPrevious(MUEM_H_2__EM0, MUEM_IM_2__EM0);
    MUEM_H_2__EM0->setObjectLink<xAOD::ElectronContainer>(featureString(), rec_2__em0_link);
    addDecisionID(HLT_mu_em_chain, MUEM_H_2__EM0);

    Decision* MUEM_CH_2__EM0 = newDecisionIn(decisionContainerPtr, "MUEM_CH_2__EM0");
    linkToPrevious(MUEM_CH_2__EM0, MUEM_H_2__EM0);
    addDecisionID(HLT_mu_em_chain, MUEM_CH_2__EM0);
    // HLT_mu_em_chain passes the event
    addDecisionID(HLT_mu_em_chain, END);
    linkToPrevious(END, MUEM_CH_2__EM0);
  }

  // Test the graph

  NavGraph graph_HLT_mufast_chain;
  NavGraph graph_HLT_mu_chain;
  NavGraph graph_HLT_mu_em_chain;
  NavGraph graph_HLT_em_chain;
  NavGraph graph_HLT_all;

  recursiveGetDecisions(END, graph_HLT_mufast_chain, HLT_mufast_chain, true);
  recursiveGetDecisions(END, graph_HLT_mu_chain, HLT_mu_chain, true);
  recursiveGetDecisions(END, graph_HLT_mu_em_chain, HLT_mu_em_chain, true);
  recursiveGetDecisions(END, graph_HLT_em_chain, HLT_em_chain, true);
  recursiveGetDecisions(END, graph_HLT_all, 0, true);


  log << MSG::INFO << "HLT_mufast_chain" << endmsg;
  graph_HLT_mufast_chain.printAllPaths(log, MSG::INFO);
  log << MSG::INFO << "HLT_mu_chain" << endmsg;
  graph_HLT_mu_chain.printAllPaths(log, MSG::INFO);
  log << MSG::INFO << "HLT_mu_em_chain" << endmsg;
  graph_HLT_mu_em_chain.printAllPaths(log, MSG::INFO);
  log << MSG::INFO << "HLT_em_chain" << endmsg;
  graph_HLT_em_chain.printAllPaths(log, MSG::INFO);
  log << MSG::INFO << "All" << endmsg;
  graph_HLT_all.printAllPaths(log, MSG::INFO);

  std::vector< LinkInfo<xAOD::IParticleContainer> > features_pass_all_HLT_mufast_chain = recursiveGetFeaturesOfType<xAOD::IParticleContainer>(graph_HLT_mufast_chain, "", false, featureString(), mufast_IDcont);
  std::vector< LinkInfo<xAOD::IParticleContainer> > features_pass_all_HLT_mu_chain     = recursiveGetFeaturesOfType<xAOD::IParticleContainer>(graph_HLT_mu_chain, "", false, featureString(), mu_IDcont);
  std::vector< LinkInfo<xAOD::IParticleContainer> > features_pass_all_HLT_mu_em_chain  = recursiveGetFeaturesOfType<xAOD::IParticleContainer>(graph_HLT_mu_em_chain, "", false, featureString(), mu_em_IDcont);
  std::vector< LinkInfo<xAOD::IParticleContainer> > features_pass_all_HLT_em_chain     = recursiveGetFeaturesOfType<xAOD::IParticleContainer>(graph_HLT_em_chain, "", false, featureString(), em_IDcont);
  std::vector< LinkInfo<xAOD::IParticleContainer> > features_pass_all_HLT_all          = recursiveGetFeaturesOfType<xAOD::IParticleContainer>(graph_HLT_all, "", false, featureString(), all_IDcont);

  printFeatures(features_pass_all_HLT_mufast_chain, "[All passing features] HLT_mufast_chain", log);
  printFeatures(features_pass_all_HLT_mu_chain, "[All passing features] HLT_mu_chain", log);
  printFeatures(features_pass_all_HLT_mu_em_chain, "[All passing features] HLT_mu_em_chain", log);
  printFeatures(features_pass_all_HLT_em_chain, "[All passing features] HLT_em_chain", log);
  printFeatures(features_pass_all_HLT_all, "[All passing features] All chains", log);

  std::vector< LinkInfo<xAOD::IParticleContainer> > features_pass_final_HLT_mufast_chain = recursiveGetFeaturesOfType<xAOD::IParticleContainer>(graph_HLT_mufast_chain, "", true, featureString(), mufast_IDcont);
  std::vector< LinkInfo<xAOD::IParticleContainer> > features_pass_final_HLT_mu_chain     = recursiveGetFeaturesOfType<xAOD::IParticleContainer>(graph_HLT_mu_chain, "", true, featureString(), mu_IDcont);
  std::vector< LinkInfo<xAOD::IParticleContainer> > features_pass_final_HLT_mu_em_chain  = recursiveGetFeaturesOfType<xAOD::IParticleContainer>(graph_HLT_mu_em_chain, "", true, featureString(), mu_em_IDcont);
  std::vector< LinkInfo<xAOD::IParticleContainer> > features_pass_final_HLT_em_chain     = recursiveGetFeaturesOfType<xAOD::IParticleContainer>(graph_HLT_em_chain, "", true, featureString(), em_IDcont);
  std::vector< LinkInfo<xAOD::IParticleContainer> > features_pass_final_HLT_all          = recursiveGetFeaturesOfType<xAOD::IParticleContainer>(graph_HLT_all, "", true, featureString(), all_IDcont);

  printFeatures(features_pass_final_HLT_mufast_chain, "[Final passing feature] HLT_mufast_chain", log);
  printFeatures(features_pass_final_HLT_mu_chain, "[Final passing feature] HLT_mu_chain", log);
  printFeatures(features_pass_final_HLT_mu_em_chain, "[Final passing feature] HLT_mu_em_chain", log);
  printFeatures(features_pass_final_HLT_em_chain, "[Final passing feature] HLT_em_chain", log);
  printFeatures(features_pass_final_HLT_all, "[Final passing feature] All chains", log);  

  std::cout << " ---------- Now Include Failing Features " << std::endl;

  std::vector<const Decision*> extraStart_HLT_mufast_chain = getRejectedDecisionNodes(pSG, HLT_mufast_chain);
  std::vector<const Decision*> extraStart_HLT_mu_chain = getRejectedDecisionNodes(pSG, HLT_mu_chain);
  std::vector<const Decision*> extraStart_HLT_mu_em_chain = getRejectedDecisionNodes(pSG, HLT_mu_em_chain);
  std::vector<const Decision*> extraStart_HLT_em_chain = getRejectedDecisionNodes(pSG, HLT_em_chain);
  std::vector<const Decision*> extraStart_HLT_all = getRejectedDecisionNodes(pSG, 0);

  for (const Decision* d : extraStart_HLT_mufast_chain) {
    recursiveGetDecisions(d, graph_HLT_mufast_chain, HLT_mufast_chain, false);
  }
  for (const Decision* d : extraStart_HLT_mu_chain) {
    recursiveGetDecisions(d, graph_HLT_mu_chain, HLT_mu_chain, false);
  }
  for (const Decision* d : extraStart_HLT_mu_em_chain) {
    recursiveGetDecisions(d, graph_HLT_mu_em_chain, HLT_mu_em_chain, false);
  }
  for (const Decision* d : extraStart_HLT_em_chain) {
    recursiveGetDecisions(d, graph_HLT_em_chain, HLT_em_chain, false);
  }
  for (const Decision* d : extraStart_HLT_all) {
    recursiveGetDecisions(d, graph_HLT_all, 0, false);
  }

  log << MSG::INFO << "HLT_mufast_chain" << endmsg;
  graph_HLT_mufast_chain.printAllPaths(log, MSG::INFO);
  log << MSG::INFO << "HLT_mu_chain" << endmsg;
  graph_HLT_mu_chain.printAllPaths(log, MSG::INFO);
  log << MSG::INFO << "HLT_mu_em_chain" << endmsg;
  graph_HLT_mu_em_chain.printAllPaths(log, MSG::INFO);
  log << MSG::INFO << "HLT_em_chain" << endmsg;
  graph_HLT_em_chain.printAllPaths(log, MSG::INFO);
  log << MSG::INFO << "All" << endmsg;
  graph_HLT_all.printAllPaths(log, MSG::INFO);

  std::vector< LinkInfo<xAOD::IParticleContainer> > features_passfail_all_HLT_mufast_chain = recursiveGetFeaturesOfType<xAOD::IParticleContainer>(graph_HLT_mufast_chain, "", false, featureString(), mufast_IDcont);
  std::vector< LinkInfo<xAOD::IParticleContainer> > features_passfail_all_HLT_mu_chain     = recursiveGetFeaturesOfType<xAOD::IParticleContainer>(graph_HLT_mu_chain, "", false, featureString(), mu_IDcont);
  std::vector< LinkInfo<xAOD::IParticleContainer> > features_passfail_all_HLT_mu_em_chain  = recursiveGetFeaturesOfType<xAOD::IParticleContainer>(graph_HLT_mu_em_chain, "", false, featureString(), mu_em_IDcont);
  std::vector< LinkInfo<xAOD::IParticleContainer> > features_passfail_all_HLT_em_chain     = recursiveGetFeaturesOfType<xAOD::IParticleContainer>(graph_HLT_em_chain, "", false, featureString(), em_IDcont);
  std::vector< LinkInfo<xAOD::IParticleContainer> > features_passfail_all_HLT_all          = recursiveGetFeaturesOfType<xAOD::IParticleContainer>(graph_HLT_all, "", false, featureString(), all_IDcont);

  printFeatures(features_passfail_all_HLT_mufast_chain, "[All passing/failing features] HLT_mufast_chain", log);
  printFeatures(features_passfail_all_HLT_mu_chain, "[All passing/failing features] HLT_mu_chain", log);
  printFeatures(features_passfail_all_HLT_mu_em_chain, "[All passing/failing features] HLT_mu_em_chain", log);
  printFeatures(features_passfail_all_HLT_em_chain, "[All passing/failing features] HLT_em_chain", log);
  printFeatures(features_passfail_all_HLT_all, "[All passing/failing features] All chains", log);

  std::vector< LinkInfo<xAOD::IParticleContainer> > features_passfail_final_HLT_mufast_chain = recursiveGetFeaturesOfType<xAOD::IParticleContainer>(graph_HLT_mufast_chain, "", true, featureString(), mufast_IDcont);
  std::vector< LinkInfo<xAOD::IParticleContainer> > features_passfail_final_HLT_mu_chain     = recursiveGetFeaturesOfType<xAOD::IParticleContainer>(graph_HLT_mu_chain, "", true, featureString(), mu_IDcont);
  std::vector< LinkInfo<xAOD::IParticleContainer> > features_passfail_final_HLT_mu_em_chain  = recursiveGetFeaturesOfType<xAOD::IParticleContainer>(graph_HLT_mu_em_chain, "", true, featureString(), mu_em_IDcont);
  std::vector< LinkInfo<xAOD::IParticleContainer> > features_passfail_final_HLT_em_chain     = recursiveGetFeaturesOfType<xAOD::IParticleContainer>(graph_HLT_em_chain, "", true, featureString(), em_IDcont);
  std::vector< LinkInfo<xAOD::IParticleContainer> > features_passfail_final_HLT_all          = recursiveGetFeaturesOfType<xAOD::IParticleContainer>(graph_HLT_all, "", true, featureString(), all_IDcont);

  printFeatures(features_passfail_final_HLT_mufast_chain, "[Final passing/failing feature] HLT_mufast_chain", log);
  printFeatures(features_passfail_final_HLT_mu_chain, "[Final passing/failing feature] HLT_mu_chain", log);
  printFeatures(features_passfail_final_HLT_mu_em_chain, "[Final passing/failing feature] HLT_mu_em_chain", log);
  printFeatures(features_passfail_final_HLT_em_chain, "[Final passing/failing feature] HLT_em_chain", log);
  printFeatures(features_passfail_final_HLT_all, "[Final passing/failing feature] All chains", log);  


  std::cout << " ---------- Check Explicit Type " << std::endl;

  // Check typed retrieval too
  // Note we are *not* passing the set of interested chains here so expect the state to be unset
  std::vector< LinkInfo<xAOD::MuonContainer> >     features_final_mu  = recursiveGetFeaturesOfType<xAOD::MuonContainer>(graph_HLT_mu_em_chain);
  std::vector< LinkInfo<xAOD::ElectronContainer> > features_final_em  = recursiveGetFeaturesOfType<xAOD::ElectronContainer>(graph_HLT_mu_em_chain);
  printFeatures(features_final_mu, "[Explicit Final Muon Features] HLT_mu_em_chain", log);
  printFeatures(features_final_em, "[Explicit Final Electron Features] HLT_mu_em_chain", log);  

  // Check filtering on the collection name. Note sub-string matching, omitting the "My".
  std::vector< LinkInfo<xAOD::ElectronContainer> > features_final_em_correctContainer   = recursiveGetFeaturesOfType<xAOD::ElectronContainer>(graph_HLT_mu_em_chain, ".*ElectronContainer.*");
  std::vector< LinkInfo<xAOD::ElectronContainer> > features_final_em_incorrectContainer = recursiveGetFeaturesOfType<xAOD::ElectronContainer>(graph_HLT_mu_em_chain, "WrongContainerName");
  VALUE ( features_final_em_correctContainer.size() ) EXPECTED ( features_final_em.size() );
  VALUE ( features_final_em_incorrectContainer.size() ) EXPECTED ( 0 );

  // Check retrieval of a link which does NOT derive from IParticle
  END->setObjectLink<DecisionContainer>("notAnIParticle", end_link);
  EXPECT_EXCEPTION (SG::ExcCLIDMismatch, END->objectLink<xAOD::IParticleContainer>("notAnIParticle"));
  
  return 0;
  
}

template<class CONTAINER>
void printFeatures(const std::vector< TrigCompositeUtils::LinkInfo<CONTAINER> >& featureContainer, const std::string& name, MsgStream& log) {
  using namespace TrigCompositeUtils;
  std::stringstream ss;
  ss << name << " features size:" << featureContainer.size() << std::endl;
  size_t count = 0;
  for (const TrigCompositeUtils::LinkInfo<CONTAINER>& featureLinkInfo : featureContainer) {
    std::string stateStr;
    switch (featureLinkInfo.state) {
      case ActiveState::ACTIVE: stateStr = "ACTIVE"; break;
      case ActiveState::INACTIVE: stateStr = "INACTIVE"; break;
      case ActiveState::UNSET: default: stateStr = "UNSET"; break;
    }
    ss << "  Feature " << count++ << " pt:" << (*featureLinkInfo.link)->pt() << ", state:" << stateStr << std::endl;
  }
  log << MSG::INFO << ss.str() << endmsg;
}
