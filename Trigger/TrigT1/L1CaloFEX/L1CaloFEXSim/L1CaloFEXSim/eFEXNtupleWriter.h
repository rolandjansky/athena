/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//                           eFEXNtupleWriter.h  -  
//                              -------------------
//     begin                : 28 02 2020
//     email                : tong.qiu@cern.ch
//  **************************************************************************
#ifndef EFEXTUPLEWRITER_H
#define EFEXTUPLEWRITER_H
#include "AthenaBaseComps/AthAlgorithm.h"
#include "TTree.h"
#include "GaudiKernel/ITHistSvc.h"
#include "xAODTruth/TruthParticle.h"
#include <memory>
#include "L1CaloFEXSim/eFEXOutputCollection.h"

namespace LVL1 {
class eFEXNtupleWriter : public AthAlgorithm
{
public:
  // this is a standard algorithm constructor
  eFEXNtupleWriter(const std::string& name, ISvcLocator* pSvcLocator);
  ~eFEXNtupleWriter();
  // these are the functions inherited from Algorithm
  StatusCode initialize ();
  StatusCode execute ();
  StatusCode finalize ();

private:
  eFEXOutputCollection* m_eFEXOutputCollection;
  //std::shared_ptr<eFEXOutputCollection> m_eFEXOutputCollection;
  float m_eg_nTOBs;
  bool m_load_truth_jet;
  std::vector<float> m_em;
  std::vector<float> m_had;
  std::vector<float> m_truth_e_eta;
  std::vector<float> m_truth_e_phi;
  std::vector<float> m_truth_e_ET;
  std::vector<float> m_truth_jet_eta;
  std::vector<float> m_truth_jet_phi;
  std::vector<float> m_truth_jet_ET;
  std::vector<float> m_truth_tauvisible_eta;
  std::vector<float> m_truth_tauvisible_phi;
  std::vector<float> m_truth_tauvisible_ET;
  std::vector<float> m_eg_haveseed;
  std::vector<float> m_eg_eta;
  std::vector<float> m_eg_phi;
  std::vector<float> m_eg_ET;
  std::vector<float> m_eg_WstotNum;
  std::vector<float> m_eg_WstotDen;
  std::vector<float> m_eg_RetaNum;
  std::vector<float> m_eg_RetaDen;
  std::vector<float> m_eg_RhadNum;
  std::vector<float> m_eg_RhadDen;
  std::vector<float> m_tau_Iso;
  std::vector<float> m_tau_Et;
  std::vector<float> m_tau_isCentralTowerSeed;
  std::string m_jet_container_name = "AntiKt10TruthJets";
  TTree *m_myTree;
  
  StatusCode loadegAlgoVariables();
  StatusCode loadtauAlgoVariables();
  StatusCode loadTruthElectron();
  StatusCode loadTruthJets();
  StatusCode loadTruthTau();
  std::unique_ptr<TLorentzVector> visibleTauP4(const xAOD::TruthParticle*);
  std::unique_ptr<TLorentzVector> invisibleTauP4(const xAOD::TruthParticle*);
  const xAOD::TruthParticle* getMother(const xAOD::TruthParticle*, int);
};
}
#endif
