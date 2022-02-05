/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file eFEXNtupleWriter.h
 * @brief create ntuples output
 *
 * File contains an Athena algorithm which creates ntuple output for the
 * debugging and validation of L1Calo bitwise simulation.
 *
 * @author Tong Qiu (tong.qiu@cern.ch)
 * @date 04 Jun 2021
 */

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
/**
 * @class eFEXNtupleWriter
 * @brief An Athena algorithm which creates Ntuple output of L1Calo bitwise simulation.
 */
public:
  /// @brief this is a standard algorithm constructor
  eFEXNtupleWriter(const std::string& name, ISvcLocator* pSvcLocator);

  /// @brief Destructor
  ~eFEXNtupleWriter();

  /// @brief initialize the Algorithm
  StatusCode initialize ();

  /// @brief execute the Algorithm
  StatusCode execute ();

  /// @brief finalize the Algorithm
  StatusCode finalize ();

private:
  /// @brief readhandle key for eFEXOutputCollection
  SG::ReadHandleKey<LVL1::eFEXOutputCollection> m_eFEXOutputCollectionSGKey {this, "MyOutputs", "eFEXOutputCollection", "MyOutputs"};

  int m_eg_nTOBs; ///< number of e-gamma tobs
  bool m_load_truth_jet; ///< if load truth jets

  // values from the e-gamma algorithm
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

  // values from the tau algorithm
  std::vector<float> m_tau_realRCore;
  std::vector<float> m_tau_rCoreCore;
  std::vector<float> m_tau_rCoreEnv;
  std::vector<float> m_tau_rCoreWP;
  std::vector<float> m_tau_realRHad;
  std::vector<float> m_tau_rHadCore;
  std::vector<float> m_tau_rHadEnv;
  std::vector<float> m_tau_rHadWP;
  std::vector<float> m_tau_seed;
  std::vector<float> m_tau_und;
  std::vector<float> m_tau_Et;
  std::vector<float> m_tau_Eta;
  std::vector<float> m_tau_Phi;
  std::vector<float> m_tau_floatEta;
  std::vector<float> m_tau_floatPhi;
  std::vector<float> m_tau_isCentralTowerSeed;
  
  std::vector<int> m_eFex_number; ///< eFex number
  std::vector<uint32_t> m_eg_tob; ///< e-gamma TOB words

  // variables in eg TOB
  std::vector<float> m_eg_TOB_FP;
  std::vector<float> m_eg_TOB_Eta;
  std::vector<float> m_eg_TOB_Phi;
  std::vector<float> m_eg_TOB_ha;
  std::vector<float> m_eg_TOB_f3;
  std::vector<float> m_eg_TOB_Re;
  std::vector<float> m_eg_TOB_Sd;
  std::vector<float> m_eg_TOB_UnD;
  std::vector<float> m_eg_TOB_Max;
  std::vector<float> m_eg_TOB_zeros;
  std::vector<float> m_eg_TOB_energy;
  
  std::string m_jet_container_name = "AntiKt10TruthJets"; ///< truth jet type
  TTree *m_myTree;
  
  /// @brief save variables related to the e-gamma algorithm
  StatusCode loadegAlgoVariables(SG::ReadHandle<LVL1::eFEXOutputCollection>);

  /// @brief save variables in the e-gamma TOB word
  StatusCode loadegAlgoTOBs(SG::ReadHandle<LVL1::eFEXOutputCollection>);

  /// @brief save variables related to the tau algorithm
  StatusCode loadtauAlgoVariables(SG::ReadHandle<LVL1::eFEXOutputCollection>);

  /// @brief save variables related to truth electrons
  StatusCode loadTruthElectron();

  /// @brief save variables related to truth jets
  StatusCode loadTruthJets();

  /// @brief save variables related to truth taus
  StatusCode loadTruthTau();

  /// @brief calculate the 4-vector of all the visible decay products of the tau
  std::unique_ptr<TLorentzVector> visibleTauP4(const xAOD::TruthParticle*);

  /// @brief calculate the 4-vector of all the invisible decay products of the tau
  std::unique_ptr<TLorentzVector> invisibleTauP4(const xAOD::TruthParticle*);

  ///  @brief find the nearest mother particle in the decay chain with the specified particle id.
  const xAOD::TruthParticle* getMother(const xAOD::TruthParticle*, int);

  /**
   * @brief extract bits from a 32-bit TOB words
   *
   * @param in TOB word
   * @param start start position (start from 1)
   * @param end end position (start from 1)
   *
   * @return extracted bit word
   *
   */
  uint32_t getbits(uint32_t in, int start, int end);
};
}
#endif
