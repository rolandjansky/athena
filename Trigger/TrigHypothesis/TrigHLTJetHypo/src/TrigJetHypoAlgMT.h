/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TrigHLTJetHypo_TrigJetHypoAlgMT_H
#define TrigHLTJetHypo_TrigJetHypoAlgMT_H

#include <string>

#include "TrigJetHypoAlgMT.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "xAODJet/JetContainer.h"
// #include "DecisionHandling/TrigCompositeUtils.h"
#include "DecisionHandling/HypoBase.h"

#include "ITrigJetHypoToolMT.h"

/**
 * @class TrigJetHypoAlgMT
 * @brief Implements jet selection for the HLT framework
 **/
class TrigJetHypoAlgMT : public ::HypoBase {
 public: 

  TrigJetHypoAlgMT( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TrigJetHypoAlgMT(); 

  virtual StatusCode  initialize() override;
  virtual StatusCode  execute( const EventContext& context ) const override;
  virtual StatusCode  finalize() override;
 
 private:

  StatusCode decide(const xAOD::JetContainer*,
                    TrigCompositeUtils::DecisionContainer* newDecisions,
                    const TrigCompositeUtils::DecisionContainer* previousDecisions
		    /* , */
                    /* const ToolHandle<ITrigJetHypoToolMT>&xs */
		    ) const;


  ToolHandleArray<ITrigJetHypoToolMT> m_hypoTools {
    this, "HypoTools", {}, "Hypo tools"};
     
  SG::ReadHandleKey< xAOD::JetContainer > m_jetsKey {
    this, "Jets", "Jets", "jet container key, full scan"};  
  
  
}; 

#endif
