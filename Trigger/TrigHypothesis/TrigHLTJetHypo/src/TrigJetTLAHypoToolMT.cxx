/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigJetTLAHypoToolMT.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
//
// ********************************************************************

#include "TrigJetTLAHypoToolMT.h"
#include "TrigJetHypoToolHelperMT.h"

#include "GaudiKernel/StatusCode.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/xAODJetAsIJetFactory.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/lineSplitter.h"
#include "./DebugInfoCollector.h"
#include "./xAODJetCollector.h"

#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "xAODTrigger/TrigCompositeContainer.h"

using TrigCompositeUtils::DecisionID;
using TrigCompositeUtils::Decision;
using TrigCompositeUtils::DecisionContainer;
using TrigCompositeUtils::DecisionIDContainer;
using TrigCompositeUtils::decisionIDs;

TrigJetTLAHypoToolMT::TrigJetTLAHypoToolMT(const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent) :
  AthAlgTool(type, name, parent),
  m_decisionId(HLT::Identifier::fromToolName(name)){

}


TrigJetTLAHypoToolMT::~TrigJetTLAHypoToolMT(){
}

StatusCode TrigJetTLAHypoToolMT::initialize(){
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

StatusCode TrigJetTLAHypoToolMT::finalize(){
  return StatusCode::SUCCESS;
}

StatusCode
TrigJetTLAHypoToolMT::decide(std::vector<JetDecision>& jetHypoInputs) const {

  int decision_count=0;

  std::unique_ptr<ITrigJetHypoInfoCollector> infocollector(nullptr);

  // jet hypo inputs:
  // pairs of const xAOD::Jet* (first) and mutable Decision* (second)

  for (auto& pair : jetHypoInputs) { 
      
      //check that the previous decision comes from the appropriate chain
      DecisionIDContainer previousDecisionIDs;
      const auto previousDecisionEL = TrigCompositeUtils::getLinkToPrevious(pair.second).at(0); // We know that the parent HypoAlg added exactly one parent.
      decisionIDs(*previousDecisionEL, previousDecisionIDs);
      //check that the previous decision corresponding to this decisionId was positive
      if ( previousDecisionIDs.count( m_decisionId.numeric() ) > 0) {
          TrigCompositeUtils::addDecisionID(getId().numeric(), pair.second);
          ++decision_count;
      }
  }
  

  std::string msg = "TrigJetTLAHypoToolMT: Passthrough decision count " + std::to_string(decision_count); 

  ATH_MSG_DEBUG(msg);

  if (infocollector){
    infocollector->collect("TrigJetTLAHypoToolMT", msg);
    infocollector->write();
  }
  return StatusCode::SUCCESS;
}

const HLT::Identifier& TrigJetTLAHypoToolMT::getId() const{
  return m_decisionId;
} 
