// this is a -*- C++ -*- file
/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// class JetMatcherAlg
// author: Jona Bossio (jbossios@cern.ch)
// This algorithm match jets from two different jet collections
// and decorates them with isMatched_JETCOLLECTIONNAME
//////////////////////////////////////////////////////////////////

#ifndef JetMatcherAlg_H
#define JetMatcherAlg_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "AsgDataHandles/WriteDecorHandleKey.h"
#include "xAODJet/JetContainer.h"
#include "xAODTrigger/JetRoIContainer.h"
#include "xAODTrigger/jFexSRJetRoIContainer.h"

#include <functional>

class JetMatcherAlg : public AthReentrantAlgorithm {

 public:

  using AthReentrantAlgorithm::AthReentrantAlgorithm;

  JetMatcherAlg(const std::string& name, ISvcLocator* pSvcLocator);

  StatusCode initialize() override;
  StatusCode execute(const EventContext& ctx) const override;
  StatusCode finalize() override;
  

 private:

  // dR matching property
  Gaudi::Property<float> m_Rmatch           {this, "Rmatch", 0.3, "R matching"};
  // calibration scale of reference jet container
  Gaudi::Property<std::string> m_calibScale {this, "JetCalibScale", "", "Calibration Scale at which to evaluate jet pT"};
  // set m_matchL1 True if matching to an  L1 jet container is to be performed.
  // we now have more than one L1 jet copntainers of various types
  // so a boolean is no no longer adequate to control the execution flow.
  // Maintained for backward compatability.
  Gaudi::Property<bool> m_matchL1           {this, "MatchL1", false, "Whether to match an L1 jet collection or actual online/offline jets"};

  // input jet containers
  SG::ReadHandleKey<xAOD::JetContainer>    m_jetContainerKey1;
  SG::ReadHandleKey<xAOD::JetContainer>    m_jetContainerKey2;


  enum class MatchType{undeclared, xAODJet, JetRoI, jFexSRJetRoI};
  MatchType m_matchType{MatchType::undeclared};
  
  
  SG::ReadHandleKey<xAOD::JetRoIContainer> m_l1jetContainerKey1;  // for backwards compatability
  SG::ReadHandleKey<xAOD::jFexSRJetRoIContainer> m_jFexSRJetRoIKey;

  // set up decorations
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_matchedKey{this, "matched", "matched", "SG key for output matched decoration"};
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_ptDiffKey{this, "ptdiff", "ptdiff", "SG key for output pt difference decoration"};
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_energyDiffKey{this, "energydiff", "energydiff", "SG key for output energy difference decoration"};
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_massDiffKey{this, "massdiff", "massdiff", "SG key for output mass difference decoration"};
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_ptRespKey{this, "ptresp", "ptresp", "SG key for output pt response decoration"};
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_energyRespKey{this, "energyresp", "energyresp", "SG key for output energy response decoration"};
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_massRespKey{this, "massresp", "massresp", "SG key for output mass response decoration"};
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_ptRefKey{this, "ptRef", "ptRef", "SG key for output pt reference decoration"};
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_etaRefKey{this, "etaRef", "etaRef", "SG key for output eta reference decoration"};

  std::vector<std::reference_wrapper<SG::WriteDecorHandleKey<xAOD::JetContainer>>> m_jetVarHandleKeys;
  
  SG::WriteDecorHandleKey<xAOD::JetRoIContainer> m_l1matchedKey{this, "l1matched", "l1matched", "SG key for output matched decoration"};
  SG::WriteDecorHandleKey<xAOD::JetRoIContainer> m_l1ptDiffKey{this, "l1ptdiff", "l1ptdiff", "SG key for output pt difference decoration"};
  SG::WriteDecorHandleKey<xAOD::JetRoIContainer> m_l1energyDiffKey{this, "l1energydiff", "l1energydiff", "SG key for output energy difference decoration"};
  SG::WriteDecorHandleKey<xAOD::JetRoIContainer> m_l1massDiffKey{this, "l1massdiff", "l1massdiff", "SG key for output mass difference decoration"};
  SG::WriteDecorHandleKey<xAOD::JetRoIContainer> m_l1ptRespKey{this, "l1ptresp", "l1ptresp", "SG key for output pt response decoration"};
  SG::WriteDecorHandleKey<xAOD::JetRoIContainer> m_l1energyRespKey{this, "l1energyresp", "l1energyresp", "SG key for output energy response decoration"};
  SG::WriteDecorHandleKey<xAOD::JetRoIContainer> m_l1massRespKey{this, "l1massresp", "l1massresp", "SG key for output mass response decoration"};
  SG::WriteDecorHandleKey<xAOD::JetRoIContainer> m_l1ptRefKey{this, "l1ptRef", "l1ptRef", "SG key for output pt reference decoration"};
  SG::WriteDecorHandleKey<xAOD::JetRoIContainer> m_l1etaRefKey{this, "l1etaRef", "l1etaRef", "SG key for output eta reference decoration"};
  std::vector<std::reference_wrapper<SG::WriteDecorHandleKey<xAOD::JetRoIContainer>>> m_l1JetVarHandleKeys;

  
  SG::WriteDecorHandleKey<xAOD::jFexSRJetRoIContainer> m_l1jFexSRmatchedKey{this, "l1jFexSRmatched", "l1jFexSRmatched", "SG key for output matched decoration"};
  SG::WriteDecorHandleKey<xAOD::jFexSRJetRoIContainer> m_l1jFexSRptDiffKey{this, "l1jFexSRptdiff", "l1jFexSRptdiff", "SG key for output pt difference decoration"};
  SG::WriteDecorHandleKey<xAOD::jFexSRJetRoIContainer> m_l1jFexSRenergyDiffKey{this, "l1jFexSRenergydiff", "l1jFexSRenergydiff", "SG key for output energy difference decoration"};
  SG::WriteDecorHandleKey<xAOD::jFexSRJetRoIContainer> m_l1jFexSRmassDiffKey{this, "l1jFexSRmassdiff", "l1jFexSRmassdiff", "SG key for output mass difference decoration"};
  SG::WriteDecorHandleKey<xAOD::jFexSRJetRoIContainer> m_l1jFexSRptRespKey{this, "l1jFexSRptresp", "l1jFexSRptresp", "SG key for output pt response decoration"};
  SG::WriteDecorHandleKey<xAOD::jFexSRJetRoIContainer> m_l1jFexSRenergyRespKey{this, "l1jFexSRenergyresp", "l1jFexSRenergyresp", "SG key for output energy response decoration"};
  SG::WriteDecorHandleKey<xAOD::jFexSRJetRoIContainer> m_l1jFexSRmassRespKey{this, "l1jFexSRmassresp", "l1jFexSRmassresp", "SG key for output mass response decoration"};
  SG::WriteDecorHandleKey<xAOD::jFexSRJetRoIContainer> m_l1jFexSRptRefKey{this, "l1jFexSRptRef", "l1jFexSRptRef", "SG key for output pt reference decoration"};
  SG::WriteDecorHandleKey<xAOD::jFexSRJetRoIContainer> m_l1jFexSRetaRefKey{this, "l1jFexSRetaRef", "l1jFexSRetaRef", "SG key for output eta reference decoration"};
  std::vector<std::reference_wrapper<SG::WriteDecorHandleKey<xAOD::jFexSRJetRoIContainer>>> m_l1jFexSRJetVarHandleKeys;

  
  StatusCode set_xAODJet_varHandleKeys();
  StatusCode set_JetRoI_varHandleKeys();
  StatusCode set_jFexSRJetRoI_varHandleKeys();

  StatusCode initialize_varHandleKeys();
  StatusCode initialize_xAODJet_varHandleKeys(bool);
  StatusCode initialize_JetRoI_varHandleKeys(bool);
  StatusCode initialize_jFexSRJetRoI_varHandleKeys(bool);

  StatusCode GetTLV(const xAOD::Jet* jet, TLorentzVector&) const;
  StatusCode GetTLV(const xAOD::JetRoI* jet, TLorentzVector&) const;
  StatusCode GetTLV(const xAOD::jFexSRJetRoI* jet, TLorentzVector&) const;

  template <typename T>
  StatusCode jetMatching(SG::ReadHandle<DataVector<T>> jets1,
		   SG::ReadHandle<xAOD::JetContainer> jets2,
		   SG::WriteDecorHandleKey<DataVector<T>> matchedHandleKey,
		   std::vector<std::reference_wrapper<SG::WriteDecorHandleKey<DataVector<T>>>> varHandleKeys,
		   const EventContext& ctx) const;
};

#endif  
