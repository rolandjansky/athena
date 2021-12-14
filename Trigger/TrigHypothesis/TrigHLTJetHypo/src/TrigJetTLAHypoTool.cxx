/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigJetTLAHypoTool.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
//
// ********************************************************************

#include "TrigJetTLAHypoTool.h"

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

TrigJetTLAHypoTool::TrigJetTLAHypoTool(const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent) :
  AthAlgTool(type, name, parent),
  m_decisionId(HLT::Identifier::fromToolName(name)){

}


TrigJetTLAHypoTool::~TrigJetTLAHypoTool(){
}

StatusCode TrigJetTLAHypoTool::initialize(){

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

StatusCode TrigJetTLAHypoTool::finalize(){
  return StatusCode::SUCCESS;
}

StatusCode
TrigJetTLAHypoTool::decide(TrigCompositeUtils::DecisionContainer* outputDecisions) const {

  int decision_count=0;

  std::unique_ptr<ITrigJetHypoInfoCollector> infocollector(nullptr);

  // jet hypo inputs:
  // pairs of const xAOD::Jet* (first) and mutable Decision* (second)

  for (auto decision : *outputDecisions) { 
      
      //check that the previous decision comes from the appropriate chain
      DecisionIDContainer previousDecisionIDs;
      const auto previousDecisionEL = TrigCompositeUtils::getLinkToPrevious(decision).at(0); // We know that the parent HypoAlg added exactly one parent.
      decisionIDs(*previousDecisionEL, previousDecisionIDs);
      //check that the previous decision corresponding to this decisionId was positive
      if ( previousDecisionIDs.count( m_decisionId.numeric() ) > 0) {
          TrigCompositeUtils::addDecisionID(getId().numeric(), decision);
          ++decision_count;
      }
  }
  

  ATH_MSG_DEBUG("TrigJetTLAHypoTool: Passthrough decision count " + std::to_string(decision_count)); 

  if (infocollector){
    infocollector->collect("TrigJetTLAHypoTool", "TrigJetTLAHypoTool: Passthrough decision count " + std::to_string(decision_count));
    infocollector->write();
  }
  return StatusCode::SUCCESS;
}

const HLT::Identifier& TrigJetTLAHypoTool::getId() const{
  return m_decisionId;
} 
