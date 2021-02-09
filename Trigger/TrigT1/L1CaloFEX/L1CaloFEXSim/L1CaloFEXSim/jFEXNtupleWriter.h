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
  jFEXOutputCollection* m_jFEXOutputCollection;
  //std::shared_ptr m_jFEXOutputCollection;
  //std::shared_ptr<jFEXOutputCollection> m_jFEXOutputCollection;
  //float m_eg_nTOBs;
  bool m_load_truth_jet;

  std::vector<float> m_truth_smallRJet_eta;
  std::vector<float> m_truth_smallRJet_phi;
  std::vector<float> m_truth_smallRJet_ET;

  std::vector<float> m_truth_jet_eta; 
  std::vector<float> m_truth_jet_phi;
  std::vector<float> m_truth_jet_ET;  

  std::vector<float> m_em;
  std::vector<float> m_had;
  std::vector<float> m_TT_Et;
 
  std::vector<float> m_smallRJet_eta;
  std::vector<float> m_smallRJet_phi;
  std::vector<float> m_smallRJet_ET;
  std::vector<float> m_smallRJet_Sat;
  std::vector<float> m_smallRJet_nTOBs;  
  std::vector<float> m_smallRJet_isCentralTowerSeed;
 
  std::vector<float> m_smallRJetTOB_eta;
  std::vector<float> m_smallRJetTOB_phi;
  std::vector<float> m_smallRJetTOB_ET;

  std::vector<float> m_largeRJet_ET;
  std::vector<float> m_largeRJet_nTOBs;  
  std::vector<float> m_largeRJetTOB_eta;
  std::vector<float> m_largeRJetTOB_phi;
  std::vector<float> m_largeRJetTOB_ET;

//std::string m_jet_container_name = "AntiKt10TruthJets";

  TTree *m_myTree;
 
  StatusCode loadsmallRJetAlgoVariables();
  StatusCode loadlargeRJetAlgoVariables();
//  StatusCode loadTruthElectron();
//  StatusCode loadTruthJets();
//  StatusCode loadTruthTau();
 // std::unique_ptr<TLorentzVector> visibleTauP4(const xAOD::TruthParticle*);
//  std::unique_ptr<TLorentzVector> invisibleTauP4(const xAOD::TruthParticle*);
//  const xAOD::TruthParticle* getMother(const xAOD::TruthParticle*, int);
};
}
#endif
