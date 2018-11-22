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
#include <sstream>

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
  /// ROBFragments vector print helper
  std::ostream& operator<<(std::ostream& str, const IROBDataProviderSvc::VROBFRAG& robFragments) {
    for (const IROBDataProviderSvc::ROBF* robf : robFragments) {
      str << "---> ROB ID = 0x" << std::hex << robf->rob_source_id() << std::dec << std::endl
          << "     ROD ID = 0x" << std::hex << robf->rod_source_id() << std::dec << std::endl
          << "     ROD Level-1 ID = " << robf->rod_lvl1_id() << std::endl;
    }
    return str;
  }
  /// ROB ID vector print helper
  std::string robIdVecToString(const std::vector<uint32_t>& robIdVec) {
    std::ostringstream str;
    for (const uint32_t robId : robIdVec)
      str << "0x" << std::hex << robId << std::dec << " ";
    return str.str();
  }
}

// =============================================================================
// Standard constructor
// =============================================================================
MTCalibPebHypoTool::MTCalibPebHypoTool(const std::string& type, const std::string& name, const IInterface* parent)
: AthAlgTool(type,name,parent),
  m_robDataProviderSvc("ROBDataProviderSvc", name),
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
  ATH_CHECK(m_robDataProviderSvc.retrieve());
  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of AthAlgTool::finalize
// =============================================================================
StatusCode MTCalibPebHypoTool::finalize() {
  ATH_MSG_INFO("Finalising " << name());
  ATH_CHECK(m_robDataProviderSvc.release());
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
  // Prefetch or retrieve ROBs
  // ---------------------------------------------------------------------------
  for (const auto& p : m_robAccessDict) {
    // Check for timeout
    if (Athena::Timeout::instance(input.eventContext).reached()) {
      ATH_MSG_INFO("Timeout reached in ROB retrieval loop");
      break;
    }
    const std::string& instruction = p.first;
    const std::vector<uint32_t>& robs = p.second;
    if (instruction.find(":ADD:")!=std::string::npos) {
      // Prefetch ROBs
      ATH_MSG_DEBUG("Preloading ROBs: " << robIdVecToString(robs));
      m_robDataProviderSvc->addROBData(input.eventContext, robs, name()+"-ADD");
    }
    if (instruction.find(":GET:")!=std::string::npos) {
      // Retrieve ROBs
      ATH_MSG_DEBUG("Retrieving ROBs: " << robIdVecToString(robs));
      // VROBFRAG = std::vector<const eformat::ROBFragment<const uint32_t*>* >
      IROBDataProviderSvc::VROBFRAG robFragments;
      m_robDataProviderSvc->getROBData(input.eventContext, robs, robFragments, name()+"-GET");
      ATH_MSG_DEBUG("Number of ROBs retrieved: " << robFragments.size());
      if (!robFragments.empty())
        ATH_MSG_DEBUG("List of ROBs found: " << std::endl << robFragments);
    }
    if (instruction.find(":COL:")!=std::string::npos) {
      // Event building
      ATH_MSG_DEBUG("Requesting full event ROBs");
      int nrobs = m_robDataProviderSvc->collectCompleteEventData(input.eventContext, name()+"-COL");
      ATH_MSG_DEBUG("Number of ROBs retrieved: " << nrobs);
    }
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
