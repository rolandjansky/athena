/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//    gFEXNtupleWriter
//                              -------------------
//     begin                : 01 04 2021
//     email                : cecilia.tosciri@cern.ch
//***************************************************************************

#ifndef GFEXTUPLEWRITER_H
#define GFEXTUPLEWRITER_H
#include "AthenaBaseComps/AthAlgorithm.h"
#include "TTree.h"
#include "GaudiKernel/ITHistSvc.h"
#include <memory>
#include "L1CaloFEXSim/gFEXOutputCollection.h"
#include "L1CaloFEXSim/gTowerContainer.h"

#include "xAODTrigger/gFexJetRoI.h"
#include "xAODTrigger/gFexJetRoIContainer.h"
#include "xAODTrigger/gFexJetRoIAuxContainer.h"

#include "xAODTrigger/gFexGlobalRoI.h"
#include "xAODTrigger/gFexGlobalRoIContainer.h"
#include "xAODTrigger/gFexGlobalRoIAuxContainer.h"


namespace LVL1 {
class gFEXNtupleWriter : public AthAlgorithm
{
public:
  // this is a standard algorithm constructor
  gFEXNtupleWriter(const std::string& name, ISvcLocator* pSvcLocator);
  ~gFEXNtupleWriter();
  // these are the functions inherited from Algorithm
   StatusCode initialize ();
   StatusCode execute ();
   StatusCode finalize ();

private:
  //readhandle key for gFEXOutputCollection
  SG::ReadHandleKey<LVL1::gFEXOutputCollection> m_gFEXOutputCollectionSGKey {this, "MyOutputs", "gFEXOutputCollection", "MyOutputs"};
  SG::ReadHandleKey<LVL1::gTowerContainer> m_gTowerContainerSGKey {this, "MyGTowers", "gTowerContainer", "MyGTowers"};
  SG::ReadHandleKey< xAOD::gFexJetRoIContainer > m_gFexRhoOutKey {this,"Key_gFexRhoOutputContainer","L1_gFexRhoRoI","Output gFexRho"};
  SG::ReadHandleKey< xAOD::gFexJetRoIContainer > m_gFexBlockOutKey {this,"Key_gFexSRJetOutputContainer","L1_gFexSRJetRoI","Output gFexBlock"};
  SG::ReadHandleKey< xAOD::gFexJetRoIContainer > m_gFexJetOutKey {this,"Key_gFexLRJetOutputContainer","L1_gFexLRJetRoI","Output gFexJet"};
  SG::ReadHandleKey< xAOD::gFexGlobalRoIContainer > m_gScalarEJwojOutKey {this,"Key_gScalarEJwojOutputContainer","L1_gScalarEJwoj","Output Scalar MET and SumET"};
  SG::ReadHandleKey< xAOD::gFexGlobalRoIContainer > m_gMETComponentsJwojOutKey {this,"Key_gMETComponentsJwojOutputContainer","L1_gMETComponentsJwoj","Output total MET components"};
  SG::ReadHandleKey< xAOD::gFexGlobalRoIContainer > m_gMHTComponentsJwojOutKey {this,"Key_gMHTComponentsJwojOutputContainer","L1_gMHTComponentsJwoj","Output hard MET components"};
  SG::ReadHandleKey< xAOD::gFexGlobalRoIContainer > m_gMSTComponentsJwojOutKey {this,"Key_gMSTComponentsJwojOutputContainer","L1_gMSTComponentsJwoj","Output soft MET components"};

  SG::ReadHandleKey<CaloCellContainer> m_scellsCollectionSGKey {this, "SCell", "SCell", "SCell"};
  

  gFEXOutputCollection* m_gFEXOutputCollection;
  float m_jet_nTOBs;
  float m_global_nTOBs;
  bool m_load_truth_jet;

  std::vector<float> m_truth_jet_eta;
  std::vector<float> m_truth_jet_phi;
  std::vector<float> m_truth_jet_ET;

  std::vector<float> m_jet_TOB;
  std::vector<float> m_jet_TOB_Eta;
  std::vector<float> m_jet_TOB_Phi;
  std::vector<float> m_jet_TOB_ET;
  std::vector<float> m_jet_TOB_ID;
  std::vector<float> m_jet_TOB_Status;


  std::vector<float> m_global_TOB;
  std::vector<float> m_global_TOB_Quantity1;
  std::vector<float> m_global_TOB_Quantity2;
  std::vector<float> m_global_TOB_Saturation;
  std::vector<float> m_global_TOB_ID;
  std::vector<float> m_global_TOB_Status1;
  std::vector<float> m_global_TOB_Status2;

  std::string m_jet_container_name = "AntiKt10TruthJets";
  TTree *m_myTree;
  TTree *m_valiTree;

  std::vector<float> m_SC_phi;
  std::vector<float> m_SC_eta;
  std::vector<float> m_SC_et;

  std::vector<float> m_gtower_phi;
  std::vector<float> m_gtower_eta;
  std::vector<float> m_gtower_et;
  std::vector<float> m_gtower_etFloat;
  std::vector<float> m_gtower_etEMFloat;
  std::vector<float> m_gtower_etHADFloat;
  std::vector<int> m_gtower_ID;
  std::vector<int> m_posneg;


  std::vector<float> m_gRho_eta;
  std::vector<float> m_gRho_phi;
  std::vector<float> m_gRho_et;

  std::vector<float> m_gSJ_iEta;
  std::vector<float> m_gSJ_iPhi;
  std::vector<float> m_gSJ_eta;
  std::vector<float> m_gSJ_gFEXphi;
  std::vector<float> m_gSJ_phi;
  std::vector<float> m_gSJ_et;

  std::vector<float> m_gLJ_iEta;
  std::vector<float> m_gLJ_iPhi;
  std::vector<float> m_gLJ_eta;
  std::vector<float> m_gLJ_gFEXphi;
  std::vector<float> m_gLJ_phi;
  std::vector<float> m_gLJ_et;

  std::vector<float> m_gGlobal_MET;
  std::vector<float> m_gGlobal_SumET;
  std::vector<float> m_gGlobal_METx;
  std::vector<float> m_gGlobal_METy;
  std::vector<float> m_gGlobal_MHTx;
  std::vector<float> m_gGlobal_MHTy;
  std::vector<float> m_gGlobal_MSTx;
  std::vector<float> m_gGlobal_MSTy;


  StatusCode loadJetAlgoVariables(SG::ReadHandle<LVL1::gFEXOutputCollection>);
  StatusCode loadGlobalAlgoVariables(SG::ReadHandle<LVL1::gFEXOutputCollection>);
  StatusCode loadTruthElectron();
  StatusCode loadTruthJets();

};

}
#endif
