// this is a -*- C++ -*- file
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

class JetMatcherAlg : public AthReentrantAlgorithm {

 public:

  using AthReentrantAlgorithm::AthReentrantAlgorithm;

  JetMatcherAlg(const std::string& name, ISvcLocator* pSvcLocator);

  StatusCode initialize() override;
  StatusCode execute(const EventContext& ctx) const override;
  StatusCode finalize() override;

 private:

  // input jet containers
  SG::ReadHandleKey<xAOD::JetContainer> m_jetContainerKey1, m_jetContainerKey2;
  // R matching property
  Gaudi::Property<float> m_Rmatch     {this, "Rmatch", 0.3, "R matching"};
  // decorations
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_matchedKey{this, "matched", "matched", "SG key for output matched decoration"};
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_ptDiffKey{this, "ptdiff", "ptdiff", "SG key for output pt difference decoration"};

};

#endif  
