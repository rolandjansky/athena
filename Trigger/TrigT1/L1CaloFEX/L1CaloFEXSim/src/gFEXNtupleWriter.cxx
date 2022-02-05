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
#include "L1CaloFEXSim/gFEXJwoJTOB.h"
#include "L1CaloFEXSim/gFEXJwoJAlgo.h"
#include "L1CaloFEXSim/gFEXOutputCollection.h"
#include <vector>
#include "TTree.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include <memory>
#include <unordered_map>

#include "xAODJet/JetContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"


LVL1::gFEXNtupleWriter::gFEXNtupleWriter(const std::string& name, ISvcLocator* pSvcLocator): AthAlgorithm(name, pSvcLocator) { }

LVL1::gFEXNtupleWriter::~gFEXNtupleWriter() {

}

StatusCode LVL1::gFEXNtupleWriter::initialize () {
  ServiceHandle<ITHistSvc> histSvc("THistSvc",name());
  CHECK( histSvc.retrieve() );
  m_myTree = new TTree("data","data");
  CHECK( histSvc->regTree("/ANALYSIS/data",m_myTree) );

  ATH_CHECK( m_gTowerContainerSGKey.initialize() );

  ATH_CHECK( m_gFexRhoOutKey.initialize() );
  ATH_CHECK( m_gFexBlockOutKey.initialize() );
  ATH_CHECK( m_gFexJetOutKey.initialize() );

  ATH_CHECK( m_gScalarEJwojOutKey.initialize() );
  ATH_CHECK( m_gMETComponentsJwojOutKey.initialize() );
  ATH_CHECK( m_gMHTComponentsJwojOutKey.initialize() );
  ATH_CHECK( m_gMSTComponentsJwojOutKey.initialize() );

  ATH_CHECK( m_scellsCollectionSGKey.initialize() );

  ATH_CHECK( m_gFEXOutputCollectionSGKey.initialize() );
  
  m_valiTree = new TTree("valiTree","valiTree");
  CHECK( histSvc->regTree("/ANALYSIS/valiTree",m_valiTree) );
  m_valiTree->Branch ("SC_eta", &m_SC_eta);
  m_valiTree->Branch ("SC_phi", &m_SC_phi);
  m_valiTree->Branch ("SC_et", &m_SC_et);
  m_valiTree->Branch ("gtower_eta", &m_gtower_eta);
  m_valiTree->Branch ("gtower_phi", &m_gtower_phi);
  m_valiTree->Branch ("gtower_et", &m_gtower_et);
  m_valiTree->Branch ("gtower_etFloat", &m_gtower_etFloat);
  m_valiTree->Branch ("gtower_etEMFloat", &m_gtower_etEMFloat);
  m_valiTree->Branch ("gtower_etHADFloat", &m_gtower_etHADFloat);
  m_valiTree->Branch ("gtower_ID", &m_gtower_ID);
  m_valiTree->Branch ("gtower_posneg", &m_posneg); 

  m_valiTree->Branch ("gRho_eta", &m_gRho_eta);  
  m_valiTree->Branch ("gRho_phi", &m_gRho_phi);
  m_valiTree->Branch ("gRho_et", &m_gRho_et);

  m_valiTree->Branch ("gSJ_eta", &m_gSJ_eta);  
  m_valiTree->Branch ("gSJ_phi", &m_gSJ_phi);
  m_valiTree->Branch ("gSJ_et", &m_gSJ_et);

  m_valiTree->Branch ("gLJ_eta", &m_gLJ_eta);  
  m_valiTree->Branch ("gLJ_phi", &m_gLJ_phi);
  m_valiTree->Branch ("gLJ_et", &m_gLJ_et);

  m_valiTree->Branch ("gGlobal_MET", &m_gGlobal_MET);  
  m_valiTree->Branch ("gGlobal_SumET", &m_gGlobal_SumET);  
  m_valiTree->Branch ("gGlobal_METx", &m_gGlobal_METx);  
  m_valiTree->Branch ("gGlobal_METy", &m_gGlobal_METy);  
  m_valiTree->Branch ("gGlobal_MHTx", &m_gGlobal_MHTx);  
  m_valiTree->Branch ("gGlobal_MHTy", &m_gGlobal_MHTy);  
  m_valiTree->Branch ("gGlobal_MSTx", &m_gGlobal_MSTx);  
  m_valiTree->Branch ("gGlobal_MSTy", &m_gGlobal_MSTy);  

  
  m_load_truth_jet = true;

  if (m_load_truth_jet){
    m_valiTree->Branch ("truth_jet_eta",  &m_truth_jet_eta);
    m_valiTree->Branch ("truth_jet_phi",  &m_truth_jet_phi);
    m_valiTree->Branch ("truth_jet_ET",  &m_truth_jet_ET);
  }

  m_myTree->Branch ("jet_TOB", &m_jet_TOB);
  m_myTree->Branch ("jet_TOB_Eta", &m_jet_TOB_Eta);
  m_myTree->Branch ("jet_TOB_Phi", &m_jet_TOB_Phi);
  m_myTree->Branch ("jet_TOB_ET", &m_jet_TOB_ET);
  m_myTree->Branch ("jet_TOB_ID",  &m_jet_TOB_ID);
  m_myTree->Branch ("jet_TOB_Status",  &m_jet_TOB_Status);
  m_myTree->Branch ("jet_nTOBs",  &m_jet_nTOBs);

  m_myTree->Branch ("global_TOB", &m_global_TOB);
  m_myTree->Branch ("global_TOB_Quantity1", &m_global_TOB_Quantity1);
  m_myTree->Branch ("global_TOB_Quantity2", &m_global_TOB_Quantity2);
  m_myTree->Branch ("global_TOB_Saturation", &m_global_TOB_Saturation);
  m_myTree->Branch ("global_TOB_ID",  &m_global_TOB_ID);
  m_myTree->Branch ("global_TOB_Status1",  &m_global_TOB_Status1);
  m_myTree->Branch ("global_TOB_Status2",  &m_global_TOB_Status2);
  m_myTree->Branch ("global_nTOBs",  &m_global_nTOBs);

  return StatusCode::SUCCESS;
}

StatusCode LVL1::gFEXNtupleWriter::execute () { 

  SG::ReadHandle<CaloCellContainer> SCCollection =SG::ReadHandle<CaloCellContainer>(m_scellsCollectionSGKey);
  if(!SCCollection.isValid()){
    ATH_MSG_FATAL("Could not retrieve SCCollection " << m_scellsCollectionSGKey.key() );
    return StatusCode::FAILURE;
  }

  SG::ReadHandle<LVL1::gTowerContainer> gTowersHandle = SG::ReadHandle<LVL1::gTowerContainer>(m_gTowerContainerSGKey);
    if(!gTowersHandle.isValid()){
      ATH_MSG_FATAL("Could not retrieve gTowerContainer " << m_gTowerContainerSGKey.key());
      return StatusCode::FAILURE;
  }
   
  //Read objects from gFEX JetContainer
  SG::ReadHandle<xAOD::gFexJetRoIContainer> gRhoHandle = SG::ReadHandle<xAOD::gFexJetRoIContainer>(m_gFexRhoOutKey);
    if(!gRhoHandle.isValid()){
      ATH_MSG_FATAL("Could not retrieve gRhoContainer " << m_gFexRhoOutKey.key());
      return StatusCode::FAILURE;
  }

  SG::ReadHandle<xAOD::gFexJetRoIContainer> gBlockHandle = SG::ReadHandle<xAOD::gFexJetRoIContainer>(m_gFexBlockOutKey);
    if(!gBlockHandle.isValid()){
      ATH_MSG_FATAL("Could not retrieve gBlockContainer " << m_gFexBlockOutKey.key());
      return StatusCode::FAILURE;
  }
  
  SG::ReadHandle<xAOD::gFexJetRoIContainer> gJetHandle = SG::ReadHandle<xAOD::gFexJetRoIContainer>(m_gFexJetOutKey);
    if(!gJetHandle.isValid()){
      ATH_MSG_FATAL("Could not retrieve gBlockContainer " << m_gFexJetOutKey.key());
      return StatusCode::FAILURE;
  }

  //Read objects from gFEX GlobalContainer
  SG::ReadHandle<xAOD::gFexGlobalRoIContainer> gScalarEHandle = SG::ReadHandle<xAOD::gFexGlobalRoIContainer>(m_gScalarEJwojOutKey);
    if(!gScalarEHandle.isValid()){
      ATH_MSG_FATAL("Could not retrieve gBlockContainer " << m_gScalarEJwojOutKey.key());
      return StatusCode::FAILURE;
  }

  SG::ReadHandle<xAOD::gFexGlobalRoIContainer> gMETHandle = SG::ReadHandle<xAOD::gFexGlobalRoIContainer>(m_gMETComponentsJwojOutKey);
    if(!gMETHandle.isValid()){
      ATH_MSG_FATAL("Could not retrieve gBlockContainer " << m_gMETComponentsJwojOutKey.key());
      return StatusCode::FAILURE;
  }

  SG::ReadHandle<xAOD::gFexGlobalRoIContainer> gMHTHandle = SG::ReadHandle<xAOD::gFexGlobalRoIContainer>(m_gMHTComponentsJwojOutKey);
    if(!gMHTHandle.isValid()){
      ATH_MSG_FATAL("Could not retrieve gBlockContainer " << m_gMHTComponentsJwojOutKey.key());
      return StatusCode::FAILURE;
  }

  SG::ReadHandle<xAOD::gFexGlobalRoIContainer> gMSTHandle = SG::ReadHandle<xAOD::gFexGlobalRoIContainer>(m_gMSTComponentsJwojOutKey);
    if(!gMSTHandle.isValid()){
      ATH_MSG_FATAL("Could not retrieve gBlockContainer " << m_gMSTComponentsJwojOutKey.key());
      return StatusCode::FAILURE;
  }

  

  m_SC_eta.clear();
  m_SC_phi.clear();
  m_SC_et.clear();
  m_gtower_eta.clear();
  m_gtower_phi.clear();
  m_gtower_et.clear();
  m_gtower_etFloat.clear();
  m_gtower_etEMFloat.clear();
  m_gtower_etHADFloat.clear();
  m_gtower_ID.clear();
  m_posneg.clear();

  m_gRho_eta.clear();
  m_gRho_phi.clear();
  m_gRho_et.clear();

  m_gSJ_eta.clear();
  m_gSJ_phi.clear();
  m_gSJ_et.clear();

  m_gLJ_eta.clear();
  m_gLJ_phi.clear();
  m_gLJ_et.clear();

  m_gGlobal_MET.clear();
  m_gGlobal_SumET.clear();
  m_gGlobal_METx.clear();
  m_gGlobal_METy.clear();
  m_gGlobal_MHTx.clear();
  m_gGlobal_MHTy.clear();
  m_gGlobal_MSTx.clear();
  m_gGlobal_MSTy.clear();
  


  for (const auto& cell : * SCCollection){
    m_SC_eta.push_back((cell)->eta());
    m_SC_phi.push_back((cell)->phi());
    m_SC_et.push_back((cell)->energy()/cosh((cell)->eta()));
  }

  for (auto const gTower : *gTowersHandle) {
    m_gtower_eta.push_back(gTower->eta());
    m_gtower_phi.push_back(gTower->phi());
    m_gtower_et.push_back(gTower->getET());
    m_gtower_etFloat.push_back(gTower->getET_float());
    m_gtower_etEMFloat.push_back(gTower->getET_EM_float());
    m_gtower_etHADFloat.push_back(gTower->getET_HAD_float());
    m_gtower_ID.push_back(gTower->getID()); 
    m_posneg.push_back(gTower->getPosNeg());
  }

  for (auto const gRho : *gRhoHandle) {
    m_gRho_eta.push_back(gRho->iEta());
    m_gRho_phi.push_back(gRho->iPhi());
    m_gRho_et.push_back(gRho->tobEt());
  }

  for (auto const gSJ : *gBlockHandle) {
    m_gSJ_eta.push_back(gSJ->iEta());
    m_gSJ_phi.push_back(gSJ->iPhi());
    m_gSJ_et.push_back(gSJ->tobEt());
  }

  for (auto const gLJ : *gJetHandle) {
    m_gLJ_eta.push_back(gLJ->iEta());
    m_gLJ_phi.push_back(gLJ->iPhi());
    m_gLJ_et.push_back(gLJ->tobEt());
  }

  for (auto const gScalarE : *gScalarEHandle) {
    m_gGlobal_MET.push_back(gScalarE->quantityOne());
    m_gGlobal_SumET.push_back(gScalarE->quantityTwo());
  }

  for (auto const gMET : *gMETHandle) {
    m_gGlobal_METx.push_back(gMET->quantityOne());
    m_gGlobal_METy.push_back(gMET->quantityTwo());
  }

  for (auto const gMHT : *gMETHandle) {
    m_gGlobal_MHTx.push_back(gMHT->quantityOne());
    m_gGlobal_MHTy.push_back(gMHT->quantityTwo());
  }

  for (auto const gMST : *gMSTHandle) {
    m_gGlobal_MSTx.push_back(gMST->quantityOne());
    m_gGlobal_MSTy.push_back(gMST->quantityTwo());
  }

  SG::ReadHandle<LVL1::gFEXOutputCollection> gFEXOutputCollectionobj = SG::ReadHandle<LVL1::gFEXOutputCollection>(m_gFEXOutputCollectionSGKey);
    if(!gFEXOutputCollectionobj.isValid()){
      ATH_MSG_FATAL("Could not retrieve gFEXOutputCollection " << m_gFEXOutputCollectionSGKey.key());
      return StatusCode::FAILURE;
  }
  
  if (!gFEXOutputCollectionobj->getdooutput()) {
    return StatusCode::SUCCESS; 
  }

  CHECK(loadJetAlgoVariables(gFEXOutputCollectionobj));
  if (m_load_truth_jet){
    CHECK(loadTruthJets());
  }

  CHECK(loadGlobalAlgoVariables(gFEXOutputCollectionobj));

  m_valiTree->Fill();
  m_myTree->Fill();
  return StatusCode::SUCCESS;
}

StatusCode LVL1::gFEXNtupleWriter::finalize () {
  ATH_MSG_DEBUG("Finalizing " << name() << "...");
  return StatusCode::SUCCESS;
}

StatusCode LVL1::gFEXNtupleWriter::loadJetAlgoVariables(SG::ReadHandle<LVL1::gFEXOutputCollection> gFEXOutputCollectionobj) {
  m_jet_TOB.clear();
  m_jet_TOB_Eta.clear();
  m_jet_TOB_Phi.clear();
  m_jet_TOB_ET.clear();
  m_jet_TOB_ID.clear();
  m_jet_TOB_Status.clear();


  m_jet_nTOBs = gFEXOutputCollectionobj->jetsSize();
  for (int i = 0; i < gFEXOutputCollectionobj->jetsSize(); i++)
  {
    uint32_t TOB = gFEXOutputCollectionobj->getJetTob()[i];
    m_jet_TOB.push_back(TOB);
    
    std::unordered_map<std::string, float> gFEXjetvalue = (gFEXOutputCollectionobj->getJet(i));
    m_jet_TOB_Eta.push_back(gFEXjetvalue["EtaJet"]);
    m_jet_TOB_Phi.push_back(gFEXjetvalue["PhiJet"]);
    m_jet_TOB_ET.push_back(gFEXjetvalue["ETJet"]);
    m_jet_TOB_ID.push_back(gFEXjetvalue["TobIDJet"]);
    m_jet_TOB_Status.push_back(gFEXjetvalue["StatusJet"]);

  }
  return StatusCode::SUCCESS;
}

StatusCode LVL1::gFEXNtupleWriter::loadGlobalAlgoVariables(SG::ReadHandle<LVL1::gFEXOutputCollection> gFEXOutputCollectionobj) {
  m_global_TOB.clear();
  m_global_TOB_Quantity1.clear();
  m_global_TOB_Quantity2.clear();
  m_global_TOB_Saturation.clear();
  m_global_TOB_ID.clear();
  m_global_TOB_Status1.clear();
  m_global_TOB_Status2.clear();

  m_global_nTOBs = gFEXOutputCollectionobj->globalsSize();
  for (int i = 0; i < gFEXOutputCollectionobj->globalsSize(); i++)
  {
    uint32_t TOB = gFEXOutputCollectionobj->getGlobalTob()[i];
    m_global_TOB.push_back(TOB);

    std::unordered_map<std::string, float> gFEXglobalvalue = (gFEXOutputCollectionobj->getGlobal(i));
    m_global_TOB_Quantity1.push_back(gFEXglobalvalue["GlobalQuantity1"]);
    m_global_TOB_Quantity2.push_back(gFEXglobalvalue["GlobalQuantity2"]);
    m_global_TOB_Saturation.push_back(gFEXglobalvalue["SaturationGlobal"]);
    m_global_TOB_ID.push_back(gFEXglobalvalue["TobIDJet"]);
    m_global_TOB_Status1.push_back(gFEXglobalvalue["GlobalStatus1"]);
    m_global_TOB_Status2.push_back(gFEXglobalvalue["GlobalStatus2"]);
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

