/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//                           jFEXNtupleWriter.h  -  
//                              -------------------
//     begin                : 12 12 2020
//     email                : varsiha.sothilingam@cern.ch
//  **************************************************************************
#ifndef JFEXTUPLEWRITER_H
#define JFEXTUPLEWRITER_H
#include "AthenaBaseComps/AthAlgorithm.h"
#include "TTree.h"
#include "GaudiKernel/ITHistSvc.h"
#include "xAODTruth/TruthParticle.h"
#include <memory>
#include "L1CaloFEXSim/jFEXOutputCollection.h"

namespace LVL1 {
class jFEXNtupleWriter : public AthAlgorithm
{
public:
  // this is a standard algorithm constructor
  jFEXNtupleWriter(const std::string& name, ISvcLocator* pSvcLocator);
  ~jFEXNtupleWriter();
  // these are the functions inherited from Algorithm
  StatusCode initialize ();
  StatusCode execute ();
  StatusCode finalize ();

private:
  SG::ReadHandleKey<LVL1::jFEXOutputCollection> m_jFEXOutputCollectionSGKey {this, "MyOutputs", "jFEXOutputCollection", "MyOutputs"};

  std::vector<int> m_smallRJet_eta;
  std::vector<int> m_smallRJet_phi;
  std::vector<int> m_smallRJet_ET;
  std::vector<int> m_smallRJet_clusterET;

  std::vector<int> m_smallRJet_nTOBs;  
  std::vector<int> m_smallRJet_isCentralTowerSeed;
 
  std::vector<int> m_smallRJetTOB_eta;
  std::vector<int> m_smallRJetTOB_phi;
  std::vector<int> m_smallRJetTOB_ET;
  std::vector<int> m_smallRJetTOB_sat;
  std::vector<int> m_smallRJetTOB_word;
  std::vector<int> m_smallRJetTOB_jfexID;
  std::vector<int> m_smallRJetTOB_fpgaID;

  std::vector<int> m_largeRJet_eta;
  std::vector<int> m_largeRJet_phi;  
  std::vector<int> m_largeRJet_ET;
  std::vector<int> m_largeRJet_nTOBs;

  std::vector<int> m_largeRJetTOB_ET;
  std::vector<int> m_largeRJetTOB_eta;
  std::vector<int> m_largeRJetTOB_phi;
  std::vector<int> m_largeRJetTOB_sat;
  std::vector<int> m_largeRJetTOB_word;
  std::vector<int> m_largeRJetTOB_jfexID;
  std::vector<int> m_largeRJetTOB_fpgaID;

  std::vector<int> m_tau_isLocalMax;
  std::vector<int> m_tau_TT_ID;
  std::vector<int> m_tau_ET;
  std::vector<int> m_tau_clusterET;
  std::vector<int> m_tau_eta;
  std::vector<int> m_tau_phi;
  std::vector<int> m_tau_realeta;
  std::vector<int> m_tau_ISO;
  std::vector<int> m_tau_jFEXid;
  std::vector<int> m_tau_FPGAid;
  
  std::vector<int> m_tau_TOB_word;
  std::vector<int> m_tau_TOB_ET;
  std::vector<int> m_tau_TOB_eta;
  std::vector<int> m_tau_TOB_phi;
  std::vector<int> m_tau_TOB_ISO;
  std::vector<int> m_tau_TOB_Sat;
  
  //Pile-up
  std::vector<int> m_pileup_FPGAid;
  std::vector<int> m_pileup_jFEXid;
  std::vector<int> m_pileup_rho_EM;
  std::vector<int> m_pileup_rho_HAD1;
  std::vector<int> m_pileup_rho_HAD2;
  std::vector<int> m_pileup_rho_HAD3;
  std::vector<int> m_pileup_rho_FCAL;
  std::vector<std::vector<int>> m_pileup_map_ID;
  std::vector<std::vector<int>> m_pileup_map_Et_values_HAD_jet;
  std::vector<std::vector<int>> m_pileup_map_Et_values_EM_jet;
  std::vector<std::vector<int>> m_pileup_map_Et_values_Total_jet;
  std::vector<std::vector<int>> m_pileup_map_Et_values_HAD_met;
  std::vector<std::vector<int>> m_pileup_map_Et_values_EM_met;
  std::vector<std::vector<int>> m_pileup_map_Et_values_Total_met;


  TTree *m_myTree;
 
  StatusCode loadsmallRJetAlgoVariables(SG::ReadHandle<LVL1::jFEXOutputCollection>);
  StatusCode loadlargeRJetAlgoVariables(SG::ReadHandle<LVL1::jFEXOutputCollection>);
  StatusCode loadtauAlgoVariables(SG::ReadHandle<LVL1::jFEXOutputCollection>);
  StatusCode loadPileupVariables(SG::ReadHandle<LVL1::jFEXOutputCollection>);
  StatusCode loadPileupEt(SG::ReadHandle<LVL1::jFEXOutputCollection>);
  
};
}
#endif
