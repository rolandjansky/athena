/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//    gFEXOutputCollection
//                              -------------------
//     begin                : 01 04 2021
//     email                : cecilia.tosciri@cern.ch
//***************************************************************************

#include <L1CaloFEXSim/gFEXNtupleWriter.h>
#include "StoreGate/StoreGateSvc.h"
#include "L1CaloFEXSim/gFEXJetTOB.h"
#include "L1CaloFEXSim/gFEXJetAlgo.h"
#include "L1CaloFEXSim/gFEXOutputCollection.h"
#include <vector>
#include "TTree.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include <memory>
#include <unordered_map>

#include "xAODJet/JetContainer.h"


LVL1::gFEXNtupleWriter::gFEXNtupleWriter(const std::string& name, ISvcLocator* pSvcLocator): AthAlgorithm(name, pSvcLocator) { }

LVL1::gFEXNtupleWriter::~gFEXNtupleWriter() {

}

StatusCode LVL1::gFEXNtupleWriter::initialize () {
  ServiceHandle<ITHistSvc> histSvc("THistSvc",name());
  CHECK( histSvc.retrieve() );
  m_myTree = new TTree("data","data");
  CHECK( histSvc->regTree("/ANALYSIS/data",m_myTree) );

  m_load_truth_jet = false;

  if (m_load_truth_jet){
    m_myTree->Branch ("truth_jet_eta",  &m_truth_jet_eta);
    m_myTree->Branch ("truth_jet_phi",  &m_truth_jet_phi);
    m_myTree->Branch ("truth_jet_ET",  &m_truth_jet_ET);
  }

  m_myTree->Branch ("jet_TOB", &m_jet_TOB);
  m_myTree->Branch ("jet_TOB_Eta", &m_jet_TOB_Eta);
  m_myTree->Branch ("jet_TOB_Phi", &m_jet_TOB_Phi);
  m_myTree->Branch ("jet_TOB_ET", &m_jet_TOB_ET);
  m_myTree->Branch ("jet_TOB_ID",  &m_jet_TOB_ID);
  m_myTree->Branch ("jet_TOB_Status",  &m_jet_TOB_Status);
  m_myTree->Branch ("jet_nTOBs",  &m_jet_nTOBs);

  return StatusCode::SUCCESS;
}

StatusCode LVL1::gFEXNtupleWriter::execute () {
  ATH_MSG_DEBUG("==== gFEXNtupleWriter ============ execute()");
  ServiceHandle<StoreGateSvc> evtStore("StoreGateSvc/StoreGateSvc",  "arbitrary");
  CHECK(evtStore.retrieve() );

  CHECK(evtStore->retrieve(m_gFEXOutputCollection, "gFEXOutputCollection"));

  CHECK(loadJetAlgoVariables());
  if (m_load_truth_jet){
    CHECK(loadTruthJets());
  }

  m_myTree->Fill();
  return StatusCode::SUCCESS;
}

StatusCode LVL1::gFEXNtupleWriter::finalize () {
  ATH_MSG_DEBUG("Finalizing " << name() << "...");
  return StatusCode::SUCCESS;
}

StatusCode LVL1::gFEXNtupleWriter::loadJetAlgoVariables() {
  m_jet_TOB.clear();
  m_jet_TOB_Eta.clear();
  m_jet_TOB_Phi.clear();
  m_jet_TOB_ET.clear();
  m_jet_TOB_ID.clear();
  m_jet_TOB_Status.clear();


  m_jet_nTOBs = m_gFEXOutputCollection->size();
  for (int i = 0; i < m_gFEXOutputCollection->size(); i++)
  {
    uint32_t TOB = m_gFEXOutputCollection->getJetTob()[i];
    m_jet_TOB.push_back(TOB);
    std::unordered_map<std::string, float> gFEXjetvalue = (m_gFEXOutputCollection->getJet(i));
    m_jet_TOB_Eta.push_back(gFEXjetvalue["EtaJet"]);
    m_jet_TOB_Phi.push_back(gFEXjetvalue["PhiJet"]);
    m_jet_TOB_ET.push_back(gFEXjetvalue["ETJet"]);
    m_jet_TOB_ID.push_back(gFEXjetvalue["TobIDJet"]);
    m_jet_TOB_Status.push_back(gFEXjetvalue["StatusJet"]);

  }
  return StatusCode::SUCCESS;
}


StatusCode LVL1::gFEXNtupleWriter::loadTruthJets() {
  m_truth_jet_eta.clear();
  m_truth_jet_phi.clear();
  m_truth_jet_ET.clear();
  const xAOD::JetContainer* truth_jets;
  StatusCode sc = evtStore()->retrieve( truth_jets, m_jet_container_name);
  if (sc ==  StatusCode::FAILURE){
    m_jet_container_name = "InTimeAntiKt4TruthJets";
    StatusCode sc2 = evtStore()->retrieve( truth_jets, m_jet_container_name);
    if (sc2 ==  StatusCode::FAILURE){
      ATH_MSG_DEBUG("gFEXNtupleWriter::loadTruthJets() Unable to determine truth jet container");
      m_load_truth_jet = false;
      return StatusCode::SUCCESS;
    }
  }
  for (unsigned i=0; i!=truth_jets->size(); i++) {
    const xAOD::Jet* each_jet = (*truth_jets)[i];
    if(each_jet->pt()<10000) continue;
    m_truth_jet_eta.push_back(each_jet->p4().Eta());
    m_truth_jet_phi.push_back(each_jet->p4().Phi());
    m_truth_jet_ET.push_back(each_jet->p4().Et());
  }
  return StatusCode::SUCCESS;
}

