/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigStreamerHypoTool.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigStreamer
//
//
// ********************************************************************

#include "TrigStreamerHypoTool.h"


TrigStreamerHypoTool::TrigStreamerHypoTool(const std::string& type,
                  const std::string& name,
                  const IInterface* parent):
  base_class(type, name, parent),
  m_decisionId(HLT::Identifier::fromToolName(name)) {}


TrigStreamerHypoTool::~TrigStreamerHypoTool() {}


StatusCode TrigStreamerHypoTool::initialize() {
  ATH_MSG_DEBUG("Initializing TrigStreamerHypoTool for " << name());
  return StatusCode::SUCCESS;
}

StatusCode TrigStreamerHypoTool::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode TrigStreamerHypoTool::decide(std::vector<ITrigStreamerHypoTool::HypoInfo>& hypoInfo) const
{
  ATH_MSG_DEBUG("Executing decide() of " << name() << " over " << hypoInfo.size() << " Decision Objects" );
  if ( not m_pass ) {
    ATH_MSG_DEBUG("Pass option is false, rejecting");
    return StatusCode::SUCCESS;
  }
  size_t count = 0;
  for (ITrigStreamerHypoTool::HypoInfo& hi : hypoInfo) {
    // Perform logic-flow check (this HypoTool can only accept the chain if the chain was active also in the previous decision object)
    if (TrigCompositeUtils::passed(getId().numeric(), hi.m_previousDecisionIDs)) {
      // There is no other pass/fail logic - this is a streamer, we accept unconditionally
      ATH_MSG_DEBUG("Decision Object at index " << count << " Passed previous trigger step, passing here too.");
      TrigCompositeUtils::addDecisionID(getId().numeric(), hi.m_newDecision);
    } else {
      ATH_MSG_DEBUG("Decision Object at index " << count << " didn't pass previous trigger step. Cannot be accepted here.");
    }
    ++count;
  }

  return StatusCode::SUCCESS;

}

const HLT::Identifier& TrigStreamerHypoTool::getId() const{
  return m_decisionId;
} 

