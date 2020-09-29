/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

*/

#ifndef TrigHLTJetHypo_TrigJetTLAHypoAlgMT_H
#define TrigHLTJetHypo_TrigJetTLAHypoAlgMT_H

#include <string>

#include "TrigJetTLAHypoAlgMT.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "xAODJet/JetContainer.h"

#include "DecisionHandling/HypoBase.h"

#include "TrigJetTLAHypoToolMT.h"

/**
 * @class TrigJetTLAHypoAlgMT
 * @brief HypoAlg for TLA algorithm
 * @details HypoAlg needed to associate trigger navigation to jets seleected for writing out by TLA algorithm 
 * @author Antonio Boveia
 * @author Caterina Doglioni
 * @author Teng Jian Khoo
 * @author Tim Martin
 * @author Rafal Bielski
 **/

class TrigJetTLAHypoAlgMT : public ::HypoBase {
 public: 

  TrigJetTLAHypoAlgMT( const std::string& name, ISvcLocator* pSvcLocator );

  virtual StatusCode  initialize() override;
  virtual StatusCode  execute( const EventContext& context ) const override;

 private:

  ToolHandleArray<TrigJetTLAHypoToolMT> m_hypoTools {
    this, "HypoTools", {}, "Hypo tools"};
     
  SG::ReadHandleKey< xAOD::JetContainer > m_TLAjetsKey {
    this, "Jets", "Jets", "TLA jet container key"};  
  
}; 

#endif
