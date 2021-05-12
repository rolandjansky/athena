/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TrigHLTJetHypo_TrigJetHypoAlg_H
#define TrigHLTJetHypo_TrigJetHypoAlg_H

#include <string>

#include "TrigJetHypoAlg.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "xAODJet/JetContainer.h"

#include "DecisionHandling/HypoBase.h"

#include "TrigJetHypoTool.h"

/**
 * @class TrigJetHypoAlg
 * @brief Implements jet selection for the HLT framework
 **/
class TrigJetHypoAlg : public ::HypoBase {
 public: 

  TrigJetHypoAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual StatusCode  initialize() override;
  virtual StatusCode  execute( const EventContext& context ) const override;

 private:

/**
 * @brief Populate outputDecisions and run all HypoTools over the jet 
 * collection.
 * @param[in] jets Input container of reco jets
 * @param[in] previousDecision The Decision object from the previous step 
 * (L1Decoder). Jet workflow specifies there to be exactly one previous 
 * decision object.
 * @param[out] outputDecisions Output container of Decision objects. 
 * To be populated with one Decision object per jet, and these filled with IDs 
 * of passing chains.
 **/

  StatusCode decide(const xAOD::JetContainer* jets,
                    const TrigCompositeUtils::Decision* previousDecision,
                    TrigCompositeUtils::DecisionContainer* outputDecisions,
                    const EventContext& context
                    ) const;

  
  ToolHandleArray<TrigJetHypoTool> m_hypoTools {
    this, "HypoTools", {}, "Hypo tools"};
     
  SG::ReadHandleKey< xAOD::JetContainer > m_jetsKey {
    this, "Jets", "Jets", "jet container key, full scan"};  

  // For preselection, we want to only generate one decision object
  // a la L1, rather than one per jet
  Gaudi::Property<bool> m_doPresel {
    this, "DoPresel", false, "is this preselection rather than main decision?"};
  
}; 

#endif
