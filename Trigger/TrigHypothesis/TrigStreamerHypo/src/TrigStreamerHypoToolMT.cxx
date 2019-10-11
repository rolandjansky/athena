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

StatusCode TrigStreamerHypoToolMT::decide(bool& pass) const
{
	ATH_MSG_DEBUG("Executing decide() of " << name());
	pass = true;
	ATH_MSG_VERBOSE("Running " << name() << " which is a streamer, of course it passes");

	return StatusCode::SUCCESS;

}

const HLT::Identifier& TrigStreamerHypoToolMT::getId() const{
  return m_decisionId;
} 

