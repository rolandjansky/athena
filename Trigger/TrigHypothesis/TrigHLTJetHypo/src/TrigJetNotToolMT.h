/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collabnotation
*/

#ifndef TRIGJETNOTTOOLMT_H
#define TRIGJETNOTTOOLMT_H
/********************************************************************
 *
 * NAME:     TrigJetNotToolMT.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 *
 *********************************************************************/


#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsDefs.h"
#include "DecisionHandling/HLTIdentifier.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "AthenaMonitoring/GenericMonitoringTool.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsDefs.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ICleaner.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJetGrouper.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CleanerBridge.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsDefs.h"

#include "ITrigJetHypoToolMT.h"
#include "ITrigJetHypoToolConfig.h"

class TrigJetNotToolMT: public extends<AthAlgTool, ITrigJetHypoToolMT> {

 public:
  
  TrigJetNotToolMT(const std::string& type,
                    const std::string& name,
                    const IInterface* parent);
  virtual ~TrigJetNotToolMT();
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  // ITrigJetHypoToolMT interface
  virtual StatusCode
    decide(const xAOD::JetContainer*, bool& pass) const override;
  virtual const HLT::Identifier& getId() const override; 
 private:

  // Identifier is used to keep track of which tool made which decision.
  // The information is stored in the event store.
  HLT::Identifier m_decisionId;  
  
  ToolHandle<ITrigJetHypoToolMT> m_hypoTool {
    this, "hypoTool", {}, "predicate to be inverted"};


};
#endif
