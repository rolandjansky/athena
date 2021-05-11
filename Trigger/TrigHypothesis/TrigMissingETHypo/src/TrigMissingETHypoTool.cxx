/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigMissingETHypoTool.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigMissingET
//
//
// ********************************************************************

#include "TrigMissingETHypoTool.h"


TrigMissingETHypoTool::TrigMissingETHypoTool(const std::string& type,
                  const std::string& name,
                  const IInterface* parent):
	base_class(type, name, parent),
	m_decisionId(HLT::Identifier::fromToolName(name)) {}


TrigMissingETHypoTool::~TrigMissingETHypoTool() {}


StatusCode TrigMissingETHypoTool::initialize() {
	ATH_MSG_DEBUG("Initializing TrigMissingETHypoTool");
    ATH_MSG_DEBUG("MET threshold set to " << m_metThreshold );
	return StatusCode::SUCCESS;
}

StatusCode TrigMissingETHypoTool::finalize() {
	return StatusCode::SUCCESS;
}

StatusCode TrigMissingETHypoTool::decide(const xAOD::TrigMissingETContainer* metContainer, bool& pass) const
{
	ATH_MSG_DEBUG("Executing decide() of " << name());
	float ex = (*metContainer)[0]->ex();
	float ey = (*metContainer)[0]->ey();
	float met = std::hypot(ex,ey)/1000.;
	pass = met > m_metThreshold;

	ATH_MSG_VERBOSE("in " << name() << ": MET = " << met);
	if (pass)
	  ATH_MSG_VERBOSE("Event passes the " << m_metThreshold << " GeV cut");
	else 
	  ATH_MSG_VERBOSE("Event fails the " << m_metThreshold << " GeV cut");

	return StatusCode::SUCCESS;

}

const HLT::Identifier& TrigMissingETHypoTool::getId() const{
  return m_decisionId;
} 

