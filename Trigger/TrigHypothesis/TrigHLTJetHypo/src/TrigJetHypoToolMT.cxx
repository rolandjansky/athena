/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigJetHypoToolMT.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
//
// ********************************************************************

#include "TrigJetHypoToolMT.h"
#include "TrigJetHypoToolHelperMT.h"

#include "GaudiKernel/StatusCode.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/xAODJetAsIJetFactory.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/lineSplitter.h"
#include "./DebugInfoCollector.h"
#include "./xAODJetCollector.h"

#include "DecisionHandling/HLTIdentifier.h"
#include "DecisionHandling/TrigCompositeUtils.h"

using TrigCompositeUtils::DecisionID;
using TrigCompositeUtils::Decision;
using TrigCompositeUtils::DecisionContainer;

TrigJetHypoToolMT::TrigJetHypoToolMT(const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent) :
  AthAlgTool(type, name, parent),
  m_decisionId(HLT::Identifier::fromToolName(name)),
  m_eventSN(std::make_unique<EventSN>()){
}


TrigJetHypoToolMT::~TrigJetHypoToolMT(){
}

StatusCode TrigJetHypoToolMT::initialize(){
  CHECK(m_evt.initialize());
  DebugInfoCollector collector(name());
  CHECK(m_helper->getDescription(collector));
  auto s = collector.toString();
  
  for(const auto& l : lineSplitter(s)){
    ATH_MSG_INFO(l);
  }
  
  if (m_visitDebug){
    collector.write();
  }
  return StatusCode::SUCCESS;
}

StatusCode TrigJetHypoToolMT::finalize(){
  return StatusCode::SUCCESS;
}

StatusCode
TrigJetHypoToolMT::decide(const xAOD::JetContainer* jets,
                          const TrigCompositeUtils::DecisionIDContainer& previousDecisionIDs,
                          std::vector<JetDecision>& jetHypoInputs) const {

  if (not TrigCompositeUtils::passed(getId().numeric(), previousDecisionIDs)) {
    // This HypoTool's chain is not active -
    // we must not check this tool's logic.
    
    return StatusCode::SUCCESS;
  }


  HypoJetVector hypoJets(jets->size());
  
  std::transform(jets -> begin(),
                 jets -> end(),
                 hypoJets.begin(),
                 xAODJetAsIJetFactory());

  /* apply cleaning and hypothesis alg */
  ATH_MSG_DEBUG("hypo helper start... "
                << " no of jets ... " 
                << jets->size() 
                << "...");

  // steady_clock::time_point t =  steady_clock::now();
  std::unique_ptr<ITrigJetHypoInfoCollector> infocollector(nullptr);
  if(m_visitDebug){
    auto collectorName = name() + std::to_string(m_eventSN->getSN());
    infocollector.reset(new  DebugInfoCollector(collectorName));
  } 

  xAODJetCollector jetCollector;
  bool pass;
  try{
    pass = m_helper->pass(hypoJets, jetCollector, infocollector);
  } catch(std::exception& e){
    ATH_MSG_ERROR("Exception raised by the TrigJetHypoToolHelperMT: " 
                  << e.what());
    return StatusCode::FAILURE;
  }

  if (pass) {
    if (jetCollector.empty()) {
      ATH_MSG_ERROR("HypoTool passed the event for " <<
                    getId().name() <<
                    ", but did not specify which jets participated");
      return StatusCode::FAILURE;
    }

    // jet hypo inputs:
    // pairs of const xAOD::Jet* (first) and mutable Decision* (second)

    auto participating_jets = jetCollector.xAODJets();
    
    for (auto& pair : jetHypoInputs) { 
      auto it = std::find(participating_jets.begin(),
                          participating_jets.end(),
                          pair.first);
      if (it != participating_jets.end()) {

	// This jet particpated in passing the event.
        // Add this HypoTool's ID to this Decision object. 

        TrigCompositeUtils::addDecisionID(getId().numeric(), pair.second);
      }
    }
  }

  if (infocollector){
    infocollector->write();
  }

  // accumulateTime(steady_clock::now() - t);
  
  ATH_MSG_DEBUG("hypo testing done chain  "
                << " no of input jets " << jets->size()
                << " no of particlating jets " << jetCollector.size()
                << " pass " << pass );
  return StatusCode::SUCCESS;
}

const HLT::Identifier& TrigJetHypoToolMT::getId() const{
  return m_decisionId;
} 
