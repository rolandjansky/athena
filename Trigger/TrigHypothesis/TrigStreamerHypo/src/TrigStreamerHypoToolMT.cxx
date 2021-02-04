/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigStreamerHypoToolMT.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigStreamer
//
//
// ********************************************************************

#include "TrigStreamerHypoToolMT.h"


TrigStreamerHypoToolMT::TrigStreamerHypoToolMT(const std::string& type,
                  const std::string& name,
                  const IInterface* parent):
  base_class(type, name, parent),
  m_decisionId(HLT::Identifier::fromToolName(name)) {}


TrigStreamerHypoToolMT::~TrigStreamerHypoToolMT() {}


StatusCode TrigStreamerHypoToolMT::initialize() {
  ATH_MSG_DEBUG("Initializing TrigStreamerHypoToolMT");
  return StatusCode::SUCCESS;
}

StatusCode TrigStreamerHypoToolMT::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode TrigStreamerHypoToolMT::decide(std::vector<ITrigStreamerHypoToolMT::HypoInfo>& hypoInfo) const
{
  ATH_MSG_DEBUG("Executing decide() of " << name() << " over " << hypoInfo.size() << " Decision Objects" );

  size_t count = 0;
  for (ITrigStreamerHypoToolMT::HypoInfo& hi : hypoInfo) {
    // Perform logic-flow check (this HypoTool can only accept the chain if the chain was active also in the previous decision object)
    if (TrigCompositeUtils::passed(getId().numeric(), hi.m_previousDecisionIDs)) {
      // There is no other pass/fail logic - this is a streamer, we accept unconditionally
      ATH_MSG_DEBUG("Decision Object at index " << count << " Passed previous trigger step, passing here too.");
      TrigCompositeUtils::addDecisionID(getId().numeric(), hi.m_newDecision);
    } else {
      ATH_MSG_DEBUG("Decision Object at index " << count << " didn't pass previous trigger step. Cannot be acceptd here.");
    }
    ++count;
  }

  return StatusCode::SUCCESS;

}

const HLT::Identifier& TrigStreamerHypoToolMT::getId() const{
  return m_decisionId;
} 

