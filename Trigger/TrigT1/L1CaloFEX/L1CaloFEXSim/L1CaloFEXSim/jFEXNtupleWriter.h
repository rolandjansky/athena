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
  
  std::vector<int> m_largeRJet_eta;
  std::vector<int> m_largeRJet_phi;  
  std::vector<int> m_largeRJet_ET;
  std::vector<int> m_largeRJet_nTOBs;

  std::vector<int> m_largeRJetTOB_ET;
  std::vector<int> m_largeRJetTOB_eta;
  std::vector<int> m_largeRJetTOB_phi;
  std::vector<int> m_largeRJetTOB_sat;

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

//std::string m_jet_container_name = "AntiKt10TruthJets";

  TTree *m_myTree;
 
  StatusCode loadsmallRJetAlgoVariables();
  StatusCode loadlargeRJetAlgoVariables();
  StatusCode loadtauAlgoVariables();
};
}
#endif
