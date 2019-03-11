/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETORTOOLMT_H
#define TRIGJETORTOOLMT_H
/********************************************************************
 *
 * NAME:     TrigJetOrToolMT.h
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

class TrigJetOrToolMT: public extends<AthAlgTool, ITrigJetHypoToolMT> {

 public:
  
  TrigJetOrToolMT(const std::string& type,
                    const std::string& name,
                    const IInterface* parent);
  virtual ~TrigJetOrToolMT();
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  // ITrigJetOrToolMT interface
  virtual StatusCode
    decide(const xAOD::JetContainer*, bool& pass) const override;
  virtual const HLT::Identifier& getId() const override; 
 private:

  // Identifier is used to keep track of which tool made which decision.
  // The information is stored in the event store.
  HLT::Identifier m_decisionId;  
  
  ToolHandle<ITrigJetHypoToolMT> m_lhs {
    this, "lhs", {}, "LHS boolean binary expression"}; 
  ToolHandle<ITrigJetHypoToolMT> m_rhs {
    this, "rhs", {}, "RHS boolean binary expression"}; 

 
 // Monitored variables...
 /*
  declareMonitoredVariable("NJet", m_njet);
  declareMonitoredVariable("Et", m_et);
  declareMonitoredVariable("Eta", m_eta);
  declareMonitoredVariable("Phi", m_phi);
*/


};
#endif
