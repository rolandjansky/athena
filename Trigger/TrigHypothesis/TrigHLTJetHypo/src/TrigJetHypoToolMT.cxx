/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigJetHypoToolMT.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
//
// ********************************************************************

#include "TrigJetHypoToolMT.h"

#include "GaudiKernel/StatusCode.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/xAODJetAsIJetFactory.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/lineSplitter.h"
#include "./DebugInfoCollector.h"
#include "./xAODJetCollector.h"

#include "TrigCompositeUtils/TrigCompositeUtils.h"

#include <sstream>

using TrigCompositeUtils::DecisionID;
using TrigCompositeUtils::Decision;
using TrigCompositeUtils::DecisionContainer;

TrigJetHypoToolMT::TrigJetHypoToolMT(const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent) :
  AthAlgTool(type, name, parent),
  m_decisionID(HLT::Identifier::fromToolName(name)),
  m_eventSN(std::make_unique<EventSN>()){
}


TrigJetHypoToolMT::~TrigJetHypoToolMT(){
}

StatusCode TrigJetHypoToolMT::initialize(){


  if (!(m_endLabelIndex > 0)){
    ATH_MSG_ERROR("endLabelIndex must be > 0, is " +
		  std::to_string(m_endLabelIndex));
    return StatusCode::FAILURE;
  }
  
  if (m_endLabelIndex == 1u) {
    m_decisionIDs.push_back(m_decisionID);
  } else {
    for (std::size_t i = 0; i != m_endLabelIndex; ++i){
      m_decisionIDs.push_back(TrigCompositeUtils::createLegName(m_decisionID,
								i));
    }
  }
  
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

  // need only check against the decision ID corresponding to the name
  // of this hypo. Leg indices are irrelevant at this point.
  if (not TrigCompositeUtils::passed(m_decisionID.numeric(),
				     previousDecisionIDs)) {
    return StatusCode::SUCCESS;
  }

  std::unique_ptr<ITrigJetHypoInfoCollector> infocollector(nullptr);
  std::unique_ptr<ITrigJetHypoInfoCollector> jetdumper(nullptr);
  if(m_visitDebug){
    auto collectorName = name() + "_" + std::to_string(m_eventSN->getSN());
    infocollector.reset(new  DebugInfoCollector(collectorName));
    auto jetdumperName =
      name()+"_passingjets_" + std::to_string(m_eventSN->getSN());
    jetdumper.reset(new  DebugInfoCollector(jetdumperName));
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
    }
    
    return StatusCode::SUCCESS;
  }

  CHECK(checkPassingJets(jetCollector, infocollector));
  CHECK(reportPassingJets(jetCollector, jetHypoInputs));
  
  // accumulateTime(steady_clock::now() - t);

  
  if (infocollector){

    std::string msg =
      "hypo testing done: no of input jets " + std::to_string(jets->size())
      + " no of particlating jets " + std::to_string(jetCollector.size())
      +  " pass: true";

    
    infocollector->collect("TrigJetHypoToolMT", msg);
    infocollector->write();

    std::stringstream ss;
    ss << jetCollector.hypoJets();
    jetdumper->collect("passed", ss.str());
  }
  
  return StatusCode::SUCCESS;
}


const std::vector<HLT::Identifier>& TrigJetHypoToolMT::getIDs() const{
  return m_decisionIDs;
}


const HLT::Identifier& TrigJetHypoToolMT::getID(std::size_t i) const{
  return m_decisionIDs[i];
}
    

StatusCode
TrigJetHypoToolMT::checkPassingJets(const xAODJetCollector& jetCollector,
				    const std::unique_ptr<ITrigJetHypoInfoCollector>& infocollector) const {
  
  if (jetCollector.empty()) {
    ATH_MSG_ERROR("HypoTool passed the event for " <<
		  m_chainName << " ev " << m_eventSN->m_id <<
		  ", but did not specify which jets participated");
    return StatusCode::FAILURE;
  }

  auto legIndices = jetCollector.legInds();
  if (!(legIndices.size() == m_endLabelIndex)) {
    ATH_MSG_ERROR("inconsistent number of leg indices");
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
TrigJetHypoToolMT::reportPassingJets(const xAODJetCollector& jetCollector,
				     const std::vector<JetDecision>& jetHypoInputs) const {

  auto legIndices = jetCollector.legInds();
  for(const auto& legInd : legIndices){
    auto participating_jets = jetCollector.xAODJets(legInd);
    CHECK(reportLeg(participating_jets, jetHypoInputs, legInd));
  }

  return StatusCode::SUCCESS;
}

StatusCode
TrigJetHypoToolMT::reportLeg(const std::vector<const xAOD::Jet*>& jets,
			     const std::vector<JetDecision>& jetHypoInputs,
			     int legInd) const {

  // check if input jet is a participating jet. If so , add
  // its Decision object.
  for (auto& pair : jetHypoInputs) {
    if (!inputJetParticipates(jets, pair)){continue;}

    TrigCompositeUtils::DecisionIDContainer passingIDs; // set<uint32_t>

    // Add the Leg DecisionID.
    // Required by any downstream Steps which need to consume jets which
    // are satisfying this leg's jet cuts
    passingIDs.insert(getID(legInd).numeric());

    // Also add the Chain DecisionID. 
    // If this is a single-leg chain, then getID(legInd) == m_decisionID
    // DecisionIDContainer is a set, so will avoid any duplication
    // We always include the Chain DecisionID such that this leg may
    // pass the event at this point, if there are no further Steps.
    passingIDs.insert(m_decisionID.numeric());

    // Copy these passingIDs into the Decision Object
    // This call also performs de-duplication
    TrigCompositeUtils::insertDecisionIDs(passingIDs, pair.second);
  }

  return StatusCode::SUCCESS;
}


bool
TrigJetHypoToolMT::inputJetParticipates(const std::vector<const xAOD::Jet*>& jets,
					const JetDecision& pair) const  {
  
  return std::find(jets.begin(), jets.end(), pair.first) != jets.end();

}
