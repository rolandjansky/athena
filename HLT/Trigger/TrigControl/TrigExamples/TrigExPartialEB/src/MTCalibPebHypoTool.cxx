/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// Trigger includes
#include "MTCalibPebHypoTool.h"
#include "TrigPartialEventBuilding/PEBInfoWriterToolBase.h" // Defines the PEBInfo keys expected by StreamTagMakerTool

// Athena includes
#include "AthenaKernel/Timeout.h"

// System includes
#include <random>
#include <thread>
#include <sstream>
#include <algorithm>

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
  /// Print helper for a container with ROB/SubDet IDs
  template<typename Container>
  std::string idsToString(const Container& ids) {
    std::ostringstream str;
    for (const uint32_t id : ids)
      str << "0x" << std::hex << id << std::dec << " ";
    return str.str();
  }
  /// Append PEB information to the Decision object
  StatusCode appendPEBInfo(TrigCompositeUtils::Decision* decision,
                           std::set<uint32_t> robIDsToAdd,
                           std::set<uint32_t> subDetIDsToAdd) {
    // Merge with previous ROBs
    std::vector<uint32_t> previousRobs;
    if (decision->getDetail(PEBInfoWriterToolBase::robListKey(), previousRobs)) {
      robIDsToAdd.insert(previousRobs.begin(), previousRobs.end());
    }

    // Merge with previous SubDets
    std::vector<uint32_t> previousSubDets;
    if (decision->getDetail(PEBInfoWriterToolBase::subDetListKey(), previousSubDets)) {
      subDetIDsToAdd.insert(previousSubDets.begin(), previousSubDets.end());
    }

    // Attach the PEB Info to the decision
    std::vector<uint32_t> robVec(robIDsToAdd.begin(), robIDsToAdd.end());
    if (not decision->setDetail(PEBInfoWriterToolBase::robListKey(), robVec)) {
      return StatusCode::FAILURE;
    }
    std::vector<uint32_t> subDetVec(subDetIDsToAdd.begin(), subDetIDsToAdd.end());
    if (not decision->setDetail(PEBInfoWriterToolBase::subDetListKey(), subDetVec)) {
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }
  /// Fill a TrigCompositeContainer with random data
  void fillRandomData(xAOD::TrigCompositeContainer& data, unsigned int maxElements) {
    unsigned int nObjects = randomInteger<unsigned int>(0, maxElements);
    for (unsigned int iObj=0; iObj<nObjects; ++iObj) {
      xAOD::TrigComposite* object = new xAOD::TrigComposite;
      data.push_back(object);
      object->setName(std::string("object_")+std::to_string(iObj));
      unsigned int nAuxDataVec = randomInteger<unsigned int>(0, maxElements);
      for (unsigned int iAuxDataVec=0; iAuxDataVec<nAuxDataVec; ++iAuxDataVec) {
        xAOD::TrigComposite::Accessor<std::vector<float>> floatVec(std::string("floatVec_")+std::to_string(iAuxDataVec));
        unsigned int nValues = randomInteger<unsigned int>(0, maxElements);
        std::vector<float> values;
        for (unsigned int iValue=0; iValue<nValues; ++iValue) values.push_back( randomRealNumber<float>(0,1) );
        floatVec(*object) = values;
      }
    }
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

  // Copy keys from map<string,uint> to WriteHandleKeyArray
  std::transform(m_createRandomData.begin(),
                 m_createRandomData.end(),
                 std::back_inserter(m_randomDataWHK),
                 [](const auto& p){return p.first;});
  ATH_CHECK(m_randomDataWHK.initialize());

  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of AthAlgTool::finalize
// =============================================================================
StatusCode MTCalibPebHypoTool::finalize() {
  ATH_MSG_INFO("Finalising " << name());
  ATH_CHECK(m_robDataProviderSvc.release());
  ATH_CHECK(m_randomDataWHK.initialize());
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
      ATH_MSG_DEBUG("Preloading ROBs: " << idsToString(robs));
      m_robDataProviderSvc->addROBData(input.eventContext, robs, name()+"-ADD");
    }
    if (instruction.find(":GET:")!=std::string::npos) {
      // Retrieve ROBs
      ATH_MSG_DEBUG("Retrieving ROBs: " << idsToString(robs));
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
    std::this_thread::sleep_for(std::chrono::milliseconds(m_timeBetweenRobReqMillisec));
  }

  // ---------------------------------------------------------------------------
  // Produce random data
  // ---------------------------------------------------------------------------
  {
    using DataCont = xAOD::TrigCompositeContainer;
    using AuxCont = xAOD::TrigCompositeAuxContainer;
    for (const SG::WriteHandleKey<DataCont>& handleKey : m_randomDataWHK) {
      // Create data and aux container
      std::unique_ptr<DataCont> data = std::make_unique<DataCont>();
      std::unique_ptr<AuxCont> aux = std::make_unique<AuxCont>();
      data->setStore(aux.get());
      // Record the container in event store
      SG::WriteHandle<DataCont> handle(handleKey,input.eventContext);
      ATH_CHECK( handle.record(std::move(data),std::move(aux)) );
      ATH_MSG_DEBUG("Recorded TrigCompositeContainer " << handleKey.key() << " in event store");
      // Fill the container with random data
      unsigned int maxElements = m_createRandomData.value().at(handleKey.key());
      fillRandomData(*handle,maxElements);
    }
  }

  // ---------------------------------------------------------------------------
  // Random accept decision with PEB information
  // ---------------------------------------------------------------------------
  bool accept = randomAccept(m_acceptRate);
  if(!accept) {
    ATH_MSG_DEBUG("Decision " << m_decisionId << " is reject");
    return StatusCode::SUCCESS;
  }

  ATH_MSG_DEBUG("Decision " << m_decisionId << " is accept");
  TrigCompositeUtils::addDecisionID(m_decisionId, input.decision);

  if (m_pebRobList.empty() && m_pebSubDetList.empty()) {
    ATH_MSG_VERBOSE("Not configured to write any PEBInfo, nothing will be attached to the decision");
  }
  else {
    ATH_MSG_DEBUG("Attaching ROBs=[" << idsToString(m_pebRobList) << "] and SubDets=["
                  << idsToString(m_pebSubDetList) << "] to the decision object");
    ATH_CHECK(appendPEBInfo(input.decision,m_pebRobList,m_pebSubDetList));
  }

  return StatusCode::SUCCESS;
}
