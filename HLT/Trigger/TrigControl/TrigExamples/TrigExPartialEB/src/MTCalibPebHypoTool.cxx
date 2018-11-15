/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "MTCalibPebHypoTool.h"
#include <random>

// Local implementation-specific helper methods
namespace {
  bool randomAccept(const double acceptRate) {
      static thread_local std::mt19937 generator;
      std::uniform_real_distribution<double> distribution(0.0, 1.0);
      return (distribution(generator) < acceptRate);
  }
}

// =============================================================================
// Standard constructor
// =============================================================================
MTCalibPebHypoTool::MTCalibPebHypoTool(const std::string& type, const std::string& name, const IInterface* parent)
: AthAlgTool(type,name,parent),
  m_decisionId (HLT::Identifier::fromToolName(name)) {}

// =============================================================================
// Standard destructor
// =============================================================================
MTCalibPebHypoTool::~MTCalibPebHypoTool() {}

// =============================================================================
// Implementation of AthAlgTool::initialize
// =============================================================================
StatusCode MTCalibPebHypoTool::initialize() {
  ATH_MSG_INFO("Initialising " << name());
  return StatusCode::SUCCESS;
}

// =============================================================================
StatusCode MTCalibPebHypoTool::decide(const std::vector<MTCalibPebHypoTool::Input>& inputs) const {

  bool accept = randomAccept(m_acceptRate);
  for (auto& in : inputs) {
    if (accept) {
      ATH_MSG_DEBUG("Decision " << m_decisionId << " is accept");
      TrigCompositeUtils::addDecisionID(m_decisionId, in.decision);
    }
    else {
      ATH_MSG_DEBUG("Decision " << m_decisionId << " is reject");
    }
  }

  return StatusCode::SUCCESS;
}
