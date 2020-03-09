/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETHYPOTOOLMT_H
#define TRIGJETHYPOTOOLMT_H
/********************************************************************
 *
 * NAME:     TrigJetHypoToolMT.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 *
 *********************************************************************/


#include "DecisionHandling/HLTIdentifier.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"

#include "TrigHLTJetHypo/ITrigJetHypoToolHelperMT.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODJet/JetContainer.h"

struct EventSN{
  std::size_t m_id{0};
  std::size_t getSN(){return m_id++;}
};

class TrigJetHypoToolMT: public AthAlgTool{
  
public:

  TrigJetHypoToolMT(const std::string& type,
                      const std::string& name,
                      const IInterface* parent);

  ~TrigJetHypoToolMT();
  StatusCode initialize();
  StatusCode finalize();
  
  // typedef for passing jet - decision object assocation
  using JetDecision = std::pair<const xAOD::Jet*,TrigCompositeUtils::Decision*>;

  StatusCode
    decide(const xAOD::JetContainer* jets,
           const TrigCompositeUtils::DecisionIDContainer& previousDecisionIDs,
           std::vector<JetDecision>& jetHypoInputs) const;
  
  const HLT::Identifier& getId() const; 

 private:

  // Identifier is used to keep track of which tool made which decision.
  // The information is stored in the event store.
  HLT::Identifier m_decisionId;  


  ToolHandle<ITrigJetHypoToolHelperMT> m_helper {
    this, "helper_tool", {}, "Jet hypo helper AlgTool"};
  
  Gaudi::Property<bool>
    m_visitDebug {this, "visit_debug", false, "debug flag"};
  
  
  std::unique_ptr<EventSN> m_eventSN;
 
};
#endif
