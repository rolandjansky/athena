/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"

#include <sstream>

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

  if (m_visitDebug){

    DebugInfoCollector collector(name());
    CHECK(m_helper->getDescription(collector));
    auto s = collector.toString();
  
    for(const auto& l : lineSplitter(s)){
      ATH_MSG_INFO(l);
    }
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
    ATH_MSG_DEBUG("Previous decisionID not found for " << getId().numeric() << " - exiting.");

    return StatusCode::SUCCESS;
  }

  std::unique_ptr<ITrigJetHypoInfoCollector> infocollector(nullptr);
  if(m_visitDebug){
    auto collectorName = name() + "_" + std::to_string(m_eventSN->getSN());
    infocollector.reset(new  DebugInfoCollector(collectorName));
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
  

  xAODJetCollector jetCollector;
  bool pass;
  try{
    pass = m_helper->pass(hypoJets, jetCollector, infocollector);
  } catch(std::exception& e){
    ATH_MSG_ERROR("Exception raised by the TrigJetHypoToolHelperMT: " 
                  << e.what());
    return StatusCode::FAILURE;
  }
  

  if (!pass) {
   
    if (infocollector){

      std::string msg =
      "hypo testing done: no of input jets " + std::to_string(jets->size())
      + " no of participating jets " + std::to_string(jetCollector.size())
      +  " pass: false ";
    
      infocollector->collect("TrigJetHypoToolMT", msg);
      infocollector->write();
    }
    
    return StatusCode::SUCCESS;
  }

  CHECK(checkPassingJets(jetCollector, infocollector));
  CHECK(reportPassingJets(jetCollector, jetHypoInputs));
  
  // accumulateTime(steady_clock::now() - t);

  
  std::string msg =
    "hypo testing done: no of input jets " + std::to_string(jets->size())
    + " no of particlating jets " + std::to_string(jetCollector.size())
    +  " pass: true";

  ATH_MSG_DEBUG(msg);
  
  if (infocollector){
    infocollector->collect("TrigJetHypoToolMT", msg);
    infocollector->write();
  }
  return StatusCode::SUCCESS;
}


const HLT::Identifier& TrigJetHypoToolMT::getId() const{
  return m_decisionId;
}


StatusCode
TrigJetHypoToolMT::checkPassingJets(xAODJetCollector& jetCollector,
				std::unique_ptr<ITrigJetHypoInfoCollector>& infocollector) const {
  
  if (jetCollector.empty()) {
    ATH_MSG_ERROR("HypoTool passed the event for " <<
		  getId().name() <<
		  ", but did not specify which jets participated");
    return StatusCode::FAILURE;
  }
  
  // jet hypo inputs:
  // pairs of const xAOD::Jet* (first) and mutable Decision* (second)
  
  auto participating_jets = jetCollector.xAODJets();
  if (infocollector) {
    infocollector->
      collect(name(),
	      "no of xAODJets " + std::to_string(participating_jets.size()));
    
    auto legInds = jetCollector.legInds();
    std::stringstream ss;
    
    for(const auto& label : legInds){
      auto jets = jetCollector.xAODJets(label);
      ss << label << " [\n";
      for(const auto& j : jets){
	ss << static_cast<const void*>(j) << '\n';
      }
      ss << "]\n";
    }
    infocollector->collect(name(), ss.str());
  }
  
  return StatusCode::SUCCESS;

}

StatusCode
TrigJetHypoToolMT::reportPassingJets(xAODJetCollector& jetCollector,
				     std::vector<JetDecision>& jetHypoInputs) const {
  auto participating_jets = jetCollector.xAODJets();
  
  for (auto& pair : jetHypoInputs) {

    auto it = std::find(participating_jets.begin(),
			participating_jets.end(),
			pair.first);

    if (it == participating_jets.end()) {
      // the input jet is not a jet that contributed
      // to passing the hypo.
      continue;
    }

    // This jet particpated in passing the event.
    // Add this HypoTool's ID to this Decision object. 
    
    TrigCompositeUtils::addDecisionID(getId().numeric(), pair.second);
  }
  
  return StatusCode::SUCCESS;
}
