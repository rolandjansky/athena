/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETHYPOTOOL_H
#define TRIGJETHYPOTOOL_H
/********************************************************************
 *
 * NAME:     TrigJetHypoTool.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 *
 *********************************************************************/


#include "TrigCompositeUtils/HLTIdentifier.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"

#include "TrigHLTJetHypo/ITrigJetHypoToolHelper.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODJet/JetContainer.h"

#include <string>
#include <optional>

class TrigJetHypoTool: public AthAlgTool{
  
public:

  TrigJetHypoTool(const std::string& type,
                      const std::string& name,
                      const IInterface* parent);

  ~TrigJetHypoTool();
  StatusCode initialize();
  StatusCode finalize();
  
  // typedef for passing jet - decision object assocation
  using JetDecision = std::pair<const xAOD::Jet*,TrigCompositeUtils::Decision*>;

  StatusCode
    decide(const xAOD::JetContainer* jets,
           const TrigCompositeUtils::DecisionIDContainer& previousDecisionIDs,
           std::vector<JetDecision>& jetHypoInputs) const;
  
  const HLT::Identifier& getID(std::size_t) const; 
  const std::vector<HLT::Identifier>& getIDs() const; 

 private:

  // Identifies is used to keep track of which tool made which decision.
  // The information is stored in the event store.
  // There will be as many entries in the IDs vector as there are chain parts.
  // Howver, the zeroth entry is calculated differently for one and multi
  // chain part chains.
  
  // single chainpart ID, claculated from name of Algorithm
  // use this to determine whether the hypo should run.
  HLT::Identifier m_decisionID;

  //single and Multi chaionpart IDs
  // if single chain part then single entry, same as m_desionId
  // else one entry per chain part. the zeroth welement is calculated
  // using m_decisionID and the chain part index = 0
  // use these IDs to report participating jets "by leg"
  std::vector<HLT::Identifier> m_decisionIDs{};

  ToolHandle<ITrigJetHypoToolHelper> m_helper {
    this, "helper_tool", {}, "Jet hypo helper AlgTool"};

  Gaudi::Property<std::size_t> m_startLabelIndex {  // no of chain parts + 1
    this, "startLabelIndex", {0}, "start value for leg indices"};

  Gaudi::Property<std::size_t> m_endLabelIndex {  // no of chain parts + 1
    this, "endLabelIndex", {0}, "end value for leg indices"};

  Gaudi::Property<bool> m_visitDebug {
    this, "visit_debug", false, "debug flag"};

  Gaudi::Property<std::string> m_chainName {
    this, "chain_name", {}, "chain name"};
  
  
  mutable std::atomic<size_t> m_id = 0;
  

  StatusCode
  checkPassingJets(const xAODJetCollector&,
		   const std::unique_ptr<ITrigJetHypoInfoCollector>&) const;
    
  StatusCode
  reportPassingJets(const xAODJetCollector&,
		    const std::vector<JetDecision>& jetHypoInputs) const;

  StatusCode
  reportLeg(const std::vector<const xAOD::Jet*>& jets,
	    const std::vector<JetDecision>& jetHypoInputs,
	    int legInd) const;

  bool
  inputJetParticipates(const std::vector<const xAOD::Jet*>& jets,
		       const JetDecision& pair) const;
 
};
#endif
