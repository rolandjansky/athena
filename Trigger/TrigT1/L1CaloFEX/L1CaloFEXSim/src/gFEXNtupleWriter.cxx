/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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
#include "L1CaloFEXSim/gFEXaltMetAlgo.h"
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

  ATH_CHECK( m_gMETComponentsNoiseCutOutKey.initialize() );
  ATH_CHECK( m_gMETComponentsRmsOutKey.initialize() );
  ATH_CHECK( m_gScalarENoiseCutOutKey.initialize() );
  ATH_CHECK( m_gScalarERmsOutKey.initialize() );

  ATH_CHECK( m_scellsCollectionSGKey.initialize() );

  ATH_CHECK( m_gFEXOutputCollectionSGKey.initialize() );
  
  m_valiTree = new TTree("valiTree","valiTree");
  CHECK( histSvc->regTree("/ANALYSIS/valiTree",m_valiTree) );
  m_valiTree->Branch ("SC_eta", &m_sc_eta);
  m_valiTree->Branch ("SC_phi", &m_sc_phi);
  m_valiTree->Branch ("SC_et", &m_sc_et);
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
  m_valiTree->Branch ("gRho_tobEt", &m_gRho_tobEt);
  m_valiTree->Branch ("gRho_etMeV", &m_gRho_etMeV);
  m_valiTree->Branch ("gRho_scaleMeV", &m_gRho_etScale);

  m_valiTree->Branch ("gSJ_iEta", &m_gSJ_iEta);  
  m_valiTree->Branch ("gSJ_iPhi", &m_gSJ_iPhi);
  m_valiTree->Branch ("gSJ_eta", &m_gSJ_eta);  
  m_valiTree->Branch ("gSJ_gFEXphi", &m_gSJ_gFEXphi);
  m_valiTree->Branch ("gSJ_phi", &m_gSJ_phi);
  m_valiTree->Branch ("gSJ_phiTopo", &m_gSJ_phiTopo);
  m_valiTree->Branch ("gSJ_tobEt", &m_gSJ_tobEt);
  m_valiTree->Branch ("gSJ_etMeV", &m_gSJ_etMeV);
  m_valiTree->Branch ("gSJ_scaleMeV", &m_gSJ_etScale);

  m_valiTree->Branch ("gLJ_iEta", &m_gLJ_iEta);  
  m_valiTree->Branch ("gLJ_iPhi", &m_gLJ_iPhi);
  m_valiTree->Branch ("gLJ_eta", &m_gLJ_eta);  
  m_valiTree->Branch ("gLJ_gFEXphi", &m_gLJ_gFEXphi);
  m_valiTree->Branch ("gLJ_phi", &m_gLJ_phi);
  m_valiTree->Branch ("gLJ_phiTopo", &m_gLJ_phiTopo);
  m_valiTree->Branch ("gLJ_tobEt", &m_gLJ_tobEt);
  m_valiTree->Branch ("gLJ_etMeV", &m_gLJ_etMeV);
  m_valiTree->Branch ("gLJ_scaleMeV", &m_gLJ_etScale);

  m_valiTree->Branch ("gJwoj_MET", &m_gJwoJ_MET);  
  m_valiTree->Branch ("gJwoj_SumET", &m_gJwoJ_SumET);  
  m_valiTree->Branch ("gJwoj_METx", &m_gJwoJ_METx);  
  m_valiTree->Branch ("gJwoj_METy", &m_gJwoJ_METy);  
  m_valiTree->Branch ("gJwoj_MHTx", &m_gJwoJ_MHTx);  
  m_valiTree->Branch ("gJwoj_MHTy", &m_gJwoJ_MHTy);  
  m_valiTree->Branch ("gJwoj_MSTx", &m_gJwoJ_MSTx);  
  m_valiTree->Branch ("gJwoj_MSTy", &m_gJwoJ_MSTy);

  m_valiTree->Branch ("gNoiseCut_METx", &m_gNoiseCut_METx);  
  m_valiTree->Branch ("gNoiseCut_METy", &m_gNoiseCut_METy);
  m_valiTree->Branch ("gRms_METx", &m_gRms_METx);  
  m_valiTree->Branch ("gRms_METy", &m_gRms_METy);
  m_valiTree->Branch ("gNoiseCut_MET", &m_gNoiseCut_MET);  
  m_valiTree->Branch ("gNoiseCut_SumET", &m_gNoiseCut_SumET);  
  m_valiTree->Branch ("gRms_MET", &m_gRms_MET);  
  m_valiTree->Branch ("gRms_SumET", &m_gRms_SumET);  

  m_valiTree->Branch ("gGlobal_scaleMeV1", &m_gGlobal_etScale1); 
  m_valiTree->Branch ("gGlobal_scaleMeV2", &m_gGlobal_etScale2); 

  
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

  const EventContext& ctx = Gaudi::Hive::currentContext();

  SG::ReadHandle<CaloCellContainer> SCCollection =SG::ReadHandle<CaloCellContainer>(m_scellsCollectionSGKey,ctx);
  if(!SCCollection.isValid()){
    ATH_MSG_FATAL("Could not retrieve SCCollection " << m_scellsCollectionSGKey.key() );
    return StatusCode::FAILURE;
  }

  SG::ReadHandle<LVL1::gTowerContainer> gTowersHandle = SG::ReadHandle<LVL1::gTowerContainer>(m_gTowerContainerSGKey,ctx);
    if(!gTowersHandle.isValid()){
      ATH_MSG_FATAL("Could not retrieve gTowerContainer " << m_gTowerContainerSGKey.key());
      return StatusCode::FAILURE;
  }
   
  //Read objects from gFEX JetContainer
  SG::ReadHandle<xAOD::gFexJetRoIContainer> gRhoHandle = SG::ReadHandle<xAOD::gFexJetRoIContainer>(m_gFexRhoOutKey,ctx);
    if(!gRhoHandle.isValid()){
      ATH_MSG_FATAL("Could not retrieve gRhoContainer " << m_gFexRhoOutKey.key());
      return StatusCode::FAILURE;
  }

  SG::ReadHandle<xAOD::gFexJetRoIContainer> gBlockHandle = SG::ReadHandle<xAOD::gFexJetRoIContainer>(m_gFexBlockOutKey,ctx);
    if(!gBlockHandle.isValid()){
      ATH_MSG_FATAL("Could not retrieve gBlockContainer " << m_gFexBlockOutKey.key());
      return StatusCode::FAILURE;
  }
  
  SG::ReadHandle<xAOD::gFexJetRoIContainer> gJetHandle = SG::ReadHandle<xAOD::gFexJetRoIContainer>(m_gFexJetOutKey,ctx);
    if(!gJetHandle.isValid()){
      ATH_MSG_FATAL("Could not retrieve gJetContainer " << m_gFexJetOutKey.key());
      return StatusCode::FAILURE;
  }

  //Read objects from gFEX GlobalContainer
  SG::ReadHandle<xAOD::gFexGlobalRoIContainer> gScalarEJwojHandle = SG::ReadHandle<xAOD::gFexGlobalRoIContainer>(m_gScalarEJwojOutKey, ctx);
    if(!gScalarEJwojHandle.isValid()){
      ATH_MSG_FATAL("Could not retrieve gScalarJwojContainer " << m_gScalarEJwojOutKey.key());
      return StatusCode::FAILURE;
  }

  SG::ReadHandle<xAOD::gFexGlobalRoIContainer> gMETJwojHandle = SG::ReadHandle<xAOD::gFexGlobalRoIContainer>(m_gMETComponentsJwojOutKey, ctx);
    if(!gMETJwojHandle.isValid()){
      ATH_MSG_FATAL("Could not retrieve gMETJwojContainer " << m_gMETComponentsJwojOutKey.key());
      return StatusCode::FAILURE;
  }

  SG::ReadHandle<xAOD::gFexGlobalRoIContainer> gMHTJwojHandle = SG::ReadHandle<xAOD::gFexGlobalRoIContainer>(m_gMHTComponentsJwojOutKey, ctx);
    if(!gMHTJwojHandle.isValid()){
      ATH_MSG_FATAL("Could not retrieve gMHTJwojContainer " << m_gMHTComponentsJwojOutKey.key());
      return StatusCode::FAILURE;
  }

  SG::ReadHandle<xAOD::gFexGlobalRoIContainer> gMSTJwojHandle = SG::ReadHandle<xAOD::gFexGlobalRoIContainer>(m_gMSTComponentsJwojOutKey, ctx);
    if(!gMSTJwojHandle.isValid()){
      ATH_MSG_FATAL("Could not retrieve gMSTJwojContainer " << m_gMSTComponentsJwojOutKey.key());
      return StatusCode::FAILURE;
  }

  SG::ReadHandle<xAOD::gFexGlobalRoIContainer> gMETNoiseCutHandle = SG::ReadHandle<xAOD::gFexGlobalRoIContainer>(m_gMETComponentsNoiseCutOutKey, ctx);
    if(!gMETNoiseCutHandle.isValid()){
      ATH_MSG_FATAL("Could not retrieve gMETNoiseCutContainer " << m_gMETComponentsNoiseCutOutKey.key());
      return StatusCode::FAILURE;
  }  

  SG::ReadHandle<xAOD::gFexGlobalRoIContainer> gMETRmsHandle = SG::ReadHandle<xAOD::gFexGlobalRoIContainer>(m_gMETComponentsRmsOutKey, ctx);
    if(!gMETRmsHandle.isValid()){
      ATH_MSG_FATAL("Could not retrieve gMETRmsContainer " << m_gMETComponentsRmsOutKey.key());
      return StatusCode::FAILURE;
  }  

  SG::ReadHandle<xAOD::gFexGlobalRoIContainer> gScalarENoiseCutHandle = SG::ReadHandle<xAOD::gFexGlobalRoIContainer>(m_gScalarENoiseCutOutKey, ctx);
    if(!gScalarENoiseCutHandle.isValid()){
      ATH_MSG_FATAL("Could not retrieve gScalarENoiseCutContainer " << m_gScalarENoiseCutOutKey.key());
      return StatusCode::FAILURE;
  }

  SG::ReadHandle<xAOD::gFexGlobalRoIContainer> gScalarERmsHandle = SG::ReadHandle<xAOD::gFexGlobalRoIContainer>(m_gScalarERmsOutKey, ctx);
    if(!gScalarERmsHandle.isValid()){
      ATH_MSG_FATAL("Could not retrieve gScalarERmsContainer " << m_gScalarERmsOutKey.key());
      return StatusCode::FAILURE;
  }

  m_sc_eta.clear();
  m_sc_phi.clear();
  m_sc_et.clear();
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
  m_gRho_tobEt.clear();
  m_gRho_etMeV.clear();
  m_gRho_etScale.clear();

  m_gSJ_iEta.clear();
  m_gSJ_iPhi.clear();
  m_gSJ_eta.clear();
  m_gSJ_gFEXphi.clear();
  m_gSJ_phi.clear();
  m_gSJ_phiTopo.clear();
  m_gSJ_tobEt.clear();
  m_gSJ_etMeV.clear();
  m_gSJ_etScale.clear();  

  m_gLJ_iEta.clear();
  m_gLJ_iPhi.clear();
  m_gLJ_eta.clear();
  m_gLJ_gFEXphi.clear();
  m_gLJ_phi.clear();
  m_gLJ_phiTopo.clear();
  m_gLJ_tobEt.clear();
  m_gLJ_etMeV.clear();
  m_gLJ_etScale.clear();  
  

  m_gJwoJ_MET.clear();
  m_gJwoJ_SumET.clear();
  m_gJwoJ_METx.clear();
  m_gJwoJ_METy.clear();
  m_gJwoJ_MHTx.clear();
  m_gJwoJ_MHTy.clear();
  m_gJwoJ_MSTx.clear();
  m_gJwoJ_MSTy.clear();

  m_gNoiseCut_METx.clear();
  m_gNoiseCut_METy.clear();
  m_gRms_METx.clear();
  m_gRms_METy.clear();
  m_gNoiseCut_MET.clear();
  m_gNoiseCut_SumET.clear();
  m_gRms_MET.clear();
  m_gRms_SumET.clear();

  m_gGlobal_etScale1.clear();  
  m_gGlobal_etScale2.clear();  
  


  for (const auto& cell : * SCCollection){
    m_sc_eta.push_back((cell)->eta());
    m_sc_phi.push_back((cell)->phi());
    m_sc_et.push_back((cell)->energy()/cosh((cell)->eta()));
  }

  for (const auto& gTower : *gTowersHandle) {
    m_gtower_eta.push_back(gTower->eta());
    m_gtower_phi.push_back(gTower->phi());
    m_gtower_et.push_back(gTower->getET());
    m_gtower_etFloat.push_back(gTower->getET_float());
    m_gtower_etEMFloat.push_back(gTower->getET_EM_float());
    m_gtower_etHADFloat.push_back(gTower->getET_HAD_float());
    m_gtower_ID.push_back(gTower->getID()); 
    m_posneg.push_back(gTower->getPosNeg());
  }

  for (const auto& gRho : *gRhoHandle) {
    m_gRho_eta.push_back(gRho->iEta());
    m_gRho_phi.push_back(gRho->iPhi());
    m_gRho_tobEt.push_back(gRho->gFexTobEt());
    m_gRho_etMeV.push_back(gRho->et());
    m_gRho_etScale.push_back(gRho->tobEtScale());
  }

  for (const auto& gSJ : *gBlockHandle) {
    m_gSJ_iEta.push_back(gSJ->iEta());
    m_gSJ_iPhi.push_back(gSJ->iPhi());
    m_gSJ_eta.push_back(gSJ->eta());
    m_gSJ_gFEXphi.push_back(gSJ->phi_gFex());
    m_gSJ_phi.push_back(gSJ->phi());
    m_gSJ_phiTopo.push_back(gSJ->iPhiTopo());
    m_gSJ_tobEt.push_back(gSJ->gFexTobEt());
    m_gSJ_etMeV.push_back(gSJ->et());
    m_gSJ_etScale.push_back(gSJ->tobEtScale());

  }

  for (const auto& gLJ : *gJetHandle) {
    m_gLJ_iEta.push_back(gLJ->iEta());
    m_gLJ_iPhi.push_back(gLJ->iPhi());
    m_gLJ_eta.push_back(gLJ->eta());
    m_gLJ_gFEXphi.push_back(gLJ->phi_gFex());
    m_gLJ_phi.push_back(gLJ->phi());
    m_gLJ_phiTopo.push_back(gLJ->iPhiTopo());
    m_gLJ_tobEt.push_back(gLJ->gFexTobEt());
    m_gLJ_etMeV.push_back(gLJ->et());
    m_gLJ_etScale.push_back(gLJ->tobEtScale());

  }

  for (const auto& gScalarEJwoj : *gScalarEJwojHandle) {
    m_gJwoJ_MET.push_back(gScalarEJwoj->METquantityOne());
    m_gJwoJ_SumET.push_back(gScalarEJwoj->METquantityTwo());
    m_gGlobal_etScale1.push_back(gScalarEJwoj->tobEtScaleOne());
    m_gGlobal_etScale2.push_back(gScalarEJwoj->tobEtScaleTwo());
  }

  for (const auto& gMETJwoj : *gMETJwojHandle) {
    m_gJwoJ_METx.push_back(gMETJwoj->METquantityOne());
    m_gJwoJ_METy.push_back(gMETJwoj->METquantityTwo());
  }

  for (const auto& gMHTJwoj : *gMHTJwojHandle) {
    m_gJwoJ_MHTx.push_back(gMHTJwoj->METquantityOne());
    m_gJwoJ_MHTy.push_back(gMHTJwoj->METquantityTwo());
  }

  for (const auto& gMSTJwoj : *gMSTJwojHandle) {
    m_gJwoJ_MSTx.push_back(gMSTJwoj->METquantityOne());
    m_gJwoJ_MSTy.push_back(gMSTJwoj->METquantityTwo());
  }

  for (const auto& gMETNoiseCut : *gMETNoiseCutHandle) {
    m_gNoiseCut_METx.push_back(gMETNoiseCut->METquantityOne());
    m_gNoiseCut_METy.push_back(gMETNoiseCut->METquantityTwo());
  }

  for (const auto& gMETRms : *gMETRmsHandle) {
    m_gRms_METx.push_back(gMETRms->METquantityOne());
    m_gRms_METy.push_back(gMETRms->METquantityTwo());
  }

  for (const auto& gScalarENoiseCut : *gScalarENoiseCutHandle) {
    m_gNoiseCut_MET.push_back(gScalarENoiseCut->METquantityOne());
    m_gNoiseCut_SumET.push_back(gScalarENoiseCut->METquantityTwo());
  }

  for (const auto& gScalarERms : *gScalarERmsHandle) {
    m_gRms_MET.push_back(gScalarERms->METquantityOne());
    m_gRms_SumET.push_back(gScalarERms->METquantityTwo());
  }


  SG::ReadHandle<LVL1::gFEXOutputCollection> gFEXOutputCollectionobj = SG::ReadHandle<LVL1::gFEXOutputCollection>(m_gFEXOutputCollectionSGKey, ctx);
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

