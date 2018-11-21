/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// Trigger includes
#include "MTCalibPebHypoTool.h"

// Athena includes
#include "AthenaKernel/Timeout.h"

// System includes
#include <random>
#include <thread>

// Local implementation-specific helper methods
namespace {
  /// Returns a reference to static thread-local random number generator
  std::mt19937& threadLocalGenerator() {
    static thread_local std::random_device rd; // used only to ensure different seeds for mt19937
    static thread_local std::mt19937 generator(rd());
    return generator;
  }
  /// Basic random real number generation
  template<typename T> T randomRealNumber(const T min, const T max) {
    std::uniform_real_distribution<T> distribution(min, max);
    return distribution(threadLocalGenerator());
  }
  /// Basic random integer generation
  template<typename T> T randomInteger(const T min, const T max) {
    std::uniform_int_distribution<T> distribution(min, max);
    return distribution(threadLocalGenerator());
  }
  /// Random bool with a given true rate
  bool randomAccept(const double acceptRate) {
      return (randomRealNumber(0.0, 1.0) < acceptRate);
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
StatusCode MTCalibPebHypoTool::decide(const MTCalibPebHypoTool::Input& input) const {

  // ---------------------------------------------------------------------------
  // Burn CPU time
  // ---------------------------------------------------------------------------
  for (unsigned int iCycle = 0; iCycle < m_numBurnCycles; ++iCycle) {
    if (Athena::Timeout::instance(input.eventContext).reached()) {
      if (m_failOnTimeout) {
        ATH_MSG_ERROR("Timeout reached in CPU time burning cycle # " << iCycle+1 << " and FailOnTimeout is true");
        return StatusCode::FAILURE;
      }
      ATH_MSG_INFO("Timeout reached in CPU time burning cycle # " << iCycle+1);
      break;
    }
    unsigned int burnTime = m_burnTimeRandomly
                            ? randomInteger<unsigned int>(0, m_burnTimePerCycleMillisec)
                            : m_burnTimePerCycleMillisec.value();
    ATH_MSG_VERBOSE("CPU time burning cycle # " << iCycle+1 << ", burn time [ms] = " << burnTime);
    std::this_thread::sleep_for(std::chrono::milliseconds(burnTime));
  }

  // ---------------------------------------------------------------------------
  // Random accept decision
  // ---------------------------------------------------------------------------
  bool accept = randomAccept(m_acceptRate);
  if (accept) {
    ATH_MSG_DEBUG("Decision " << m_decisionId << " is accept");
    TrigCompositeUtils::addDecisionID(m_decisionId, input.decision);
  }
  else {
    ATH_MSG_DEBUG("Decision " << m_decisionId << " is reject");
  }

  return StatusCode::SUCCESS;
}
