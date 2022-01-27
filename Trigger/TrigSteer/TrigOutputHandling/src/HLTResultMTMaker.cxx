/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigOutputHandling/HLTResultMTMaker.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "eformat/DetectorMask.h"
#include "eformat/SourceIdentifier.h"
#include <unistd.h> // gethostname
#include <limits.h> // HOST_NAME_MAX
#include <sstream>

// Local helpers
namespace {
  /// Replaced with std::set::contains in C++20
  template<typename T> bool contains(const std::set<T>& set, const T& value) {
    return set.find(value) != set.cend();
  }
  bool isHLT(const uint32_t robid) {
    return eformat::helper::SourceIdentifier(robid).subdetector_id() == eformat::TDAQ_HLT;
  }
  bool isHLT(const eformat::SubDetector subdet) {
    return subdet == eformat::TDAQ_HLT;
  }
  /// Remove disabled IDs from @c requestedIDs and return the set of removed IDs
  template<typename T> std::set<T> removeDisabled(std::set<T>& requestedIds, const std::set<T>& enabledIds) {
    std::set<T> removedIds;
    for (auto it=requestedIds.begin(); it!=requestedIds.end(); ) {
      // Check if ROB/SubDet is disabled (skip HLT result ID)
      if (!isHLT(*it) && !contains(enabledIds, *it)) {
        removedIds.insert(*it);
        it = requestedIds.erase(it);
      }
      else ++it;
    }
    return removedIds;
  }
  /// Print helper for set<uint32_t>
  std::string format(const std::set<uint32_t>& set) {
    std::ostringstream ss;
    for (const uint32_t id : set)
      ss << "0x" << std::hex << std::setfill('0') << std::setw(8) << id << std::dec << " ";
    return ss.str();
  }
  /// Print helper for set<eformat::SubDetector>
  std::string format(const std::set<eformat::SubDetector>& set) {
    std::ostringstream ss;
    for (const eformat::SubDetector id : set) ss << eformat::helper::SubDetectorDictionary.string(id) << " ";
    return ss.str();
  }
}

// =============================================================================
// Standard constructor
// =============================================================================
HLTResultMTMaker::HLTResultMTMaker(const std::string& type, const std::string& name, const IInterface* parent)
  : AthAlgTool(type, name, parent) {}

// =============================================================================
// Implementation of IStateful::initialize
// =============================================================================
StatusCode HLTResultMTMaker::initialize() {
  ATH_CHECK(m_hltResultWHKey.initialize());
  ATH_CHECK(m_runtimeMetadataWHKey.initialize());
  ATH_CHECK(m_streamTagMaker.retrieve(DisableTool{m_streamTagMaker.name().empty()}));
  ATH_CHECK(m_makerTools.retrieve());
  ATH_CHECK(m_monTool.retrieve());
  ATH_CHECK(m_jobOptionsSvc.retrieve());
  ATH_CHECK(m_bsMetaDataContRHKey.initialize(!m_extraROBs.empty() || !m_extraSubDets.empty()));

  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of IStateful::start
// Initialise the enabled ROBs/SubDets list from DataFlowConfig and extra properties.
// =============================================================================
StatusCode HLTResultMTMaker::start() {
  // DataFlowConfig is a special object used online to hold DF properties passed from TDAQ to HLT as run parameters.
  Gaudi::Property<std::vector<uint32_t>> enabledROBsProp("EnabledROBs",{});
  if (enabledROBsProp.fromString(m_jobOptionsSvc->get("DataFlowConfig.DF_Enabled_ROB_IDs","[]")).isSuccess()) {
    m_enabledROBs.insert(enabledROBsProp.value().begin(), enabledROBsProp.value().end());
    ATH_MSG_DEBUG("Retrieved a list of " << m_enabledROBs.size()
                  << " ROBs from DataFlowConfig.DF_Enabled_ROB_IDs");
  }
  else {
    ATH_MSG_ERROR("Could not parse DataFlowConfig.DF_Enabled_ROB_IDs from JobOptionsSvc");
  }

  Gaudi::Property<std::vector<uint32_t>> enabledSubDetsProp("EnabledSubDets",{});
  if (enabledSubDetsProp.fromString(m_jobOptionsSvc->get("DataFlowConfig.DF_Enabled_SubDet_IDs","[]")).isSuccess()) {
    // Need to convert from uint32_t to eformat::SubDetector representable by uint8_t
    for (const uint32_t id : enabledSubDetsProp.value()) {
      m_enabledSubDets.insert( static_cast<eformat::SubDetector>(id & 0xFF) );
    }
    ATH_MSG_DEBUG("Retrieved a list of " << m_enabledSubDets.size()
                  << " SubDets from DataFlowConfig.DF_Enabled_SubDet_IDs");
  }
  else {
    ATH_MSG_ERROR("Could not parse DataFlowConfig.DF_Enabled_SubDet_IDs from JobOptionsSvc");
  }

  if (m_enabledROBs.empty() && m_enabledSubDets.empty()) {
    m_skipValidatePEBInfo = true;
    ATH_MSG_INFO("The list of enabled ROBs and SubDets was not provided by DataFlowConfig or is empty. "
                 << "StreamTag PEBInfo validation will be skipped.");
  }
  else {
    ATH_CHECK(validateExtraROBsAndSubDets());
    m_enabledROBs.insert(m_extraEnabledROBs.value().begin(), m_extraEnabledROBs.value().end());
    m_enabledROBs.insert(m_extraROBs.value().begin(), m_extraROBs.value().end());
    // Need to convert from uint32_t to eformat::SubDetector representable by uint8_t
    for (const uint32_t id : m_extraEnabledSubDets.value()) {
      m_enabledSubDets.insert( static_cast<eformat::SubDetector>(id & 0xFF) );
    }
    for (const uint32_t id : m_extraSubDets.value()) {
      m_enabledSubDets.insert( static_cast<eformat::SubDetector>(id & 0xFF) );
    }
    ATH_MSG_INFO("StreamTag PEBInfo will be validated against " << m_enabledROBs.size() << " enabled ROBs and "
                 << m_enabledSubDets.size() << " enabled SubDets");
  }
  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of IStateful::finalize
// =============================================================================
StatusCode HLTResultMTMaker::finalize() {
  ATH_CHECK(m_monTool.release());
  return StatusCode::SUCCESS;
}

// =============================================================================
// The main method of the tool
// =============================================================================
StatusCode HLTResultMTMaker::makeResult(const EventContext& eventContext) const {
  auto monTime =  Monitored::Timer<std::chrono::duration<float, std::milli>>("TIME_makeResult");

  // Create and record the HLTResultMT object
  auto hltResult = SG::makeHandle(m_hltResultWHKey,eventContext);
  ATH_CHECK( hltResult.record(std::make_unique<HLT::HLTResultMT>()) );
  ATH_MSG_DEBUG("Recorded HLTResultMT with key " << m_hltResultWHKey.key());

  // Save HLT runtime metadata
  SG::WriteHandle<xAOD::TrigCompositeContainer> runtimeMetadataOutput = TrigCompositeUtils::createAndStore(m_runtimeMetadataWHKey, eventContext);
  
  xAOD::TrigComposite* tc = new xAOD::TrigComposite();
  runtimeMetadataOutput->push_back(tc);
  char hostname [HOST_NAME_MAX];
  bool errcode = !gethostname(hostname, HOST_NAME_MAX); // returns 0 on success and -1 on failure, casted to false on success, true on failure
  std::string hostnameString = std::string(hostname); // setDetail needs a reference
  errcode &= tc->setDetail("hostname", hostnameString);
  if (!errcode) ATH_MSG_WARNING("Failed to append hostname to HLT Runtime Metadata TC");

  // Fill the stream tags
  StatusCode finalStatus = StatusCode::SUCCESS;
  if (m_streamTagMaker.isEnabled() && m_streamTagMaker->fill(*hltResult, eventContext).isFailure()) {
    ATH_MSG_ERROR(m_streamTagMaker->name() << " failed");
    finalStatus = StatusCode::FAILURE;
  }

  // Fill the result using all other tools if the event was accepted
  if (hltResult->isAccepted()) {
    for (auto& maker: m_makerTools) {
      ATH_MSG_DEBUG("Calling " << maker->name() << " for accepted event");
      if (StatusCode sc = maker->fill(*hltResult, eventContext); sc.isFailure()) {
        ATH_MSG_ERROR(maker->name() << " failed");
        finalStatus = sc;
      }
    }

    if (!m_skipValidatePEBInfo) validatePEBInfo(*hltResult);
  }
  else {
    ATH_MSG_DEBUG("Rejected event, further result filling skipped after stream tag maker");
  }

  ATH_MSG_DEBUG(*hltResult);

  Monitored::Group(m_monTool, monTime);
  return finalStatus;
}

// =============================================================================
// Private method removing disabled ROBs/SubDets from StreamTags
// =============================================================================
void HLTResultMTMaker::validatePEBInfo(HLT::HLTResultMT& hltResult) const {
  for (eformat::helper::StreamTag& st : hltResult.getStreamTagsNonConst()) {
    std::set<uint32_t> removedROBs = removeDisabled(st.robs,m_enabledROBs);
    if (!removedROBs.empty())
      ATH_MSG_WARNING("StreamTag " << st.type << "_" << st.name << " requested disabled ROBs: " << format(removedROBs)
                      << " - these ROBs were removed from the StreamTag by " << name());
    else
      ATH_MSG_VERBOSE("No disabled ROBs were requested by StreamTag " << st.type << "_" << st.name);

    std::set<eformat::SubDetector> removedSubDets = removeDisabled(st.dets,m_enabledSubDets);
    if (!removedSubDets.empty())
      ATH_MSG_WARNING("StreamTag " << st.type << "_" << st.name << " requested disabled SubDets: " << format(removedSubDets)
                      << " - these SubDets were removed from the StreamTag by " << name());
    else
      ATH_MSG_VERBOSE("No disabled SubDets were requested by StreamTag " << st.type << "_" << st.name);
  }
}

// =============================================================================
// Private method removing ExtraROBs/ExtraSubDets disabled by the detector mask
// =============================================================================
StatusCode HLTResultMTMaker::validateExtraROBsAndSubDets() {
  if (m_extraROBs.empty() && m_extraSubDets.empty()) {
    // Nothing to check
    return StatusCode::SUCCESS;
  }

  // Get the detector mask
  auto bsmdc = SG::makeHandle(m_bsMetaDataContRHKey);
  ATH_CHECK(bsmdc.isValid());
  ATH_CHECK(!bsmdc->empty());
  const ByteStreamMetadata* metadata = bsmdc->front();
  const eformat::helper::DetectorMask detectorMask{metadata->getDetectorMask(), metadata->getDetectorMask2()};

  // Check ROBs
  std::vector<uint32_t> validatedExtraROBs;
  for (const uint32_t rob_id : m_extraROBs.value()) {
    const eformat::helper::SourceIdentifier sid{rob_id};
    const eformat::SubDetector subdet_id = sid.subdetector_id();
    if (detectorMask.is_set(subdet_id)) {
      validatedExtraROBs.push_back(rob_id);
    }
    else {
      ATH_MSG_WARNING("Removing 0x" << MSG::hex << rob_id << MSG::dec << " from " << m_extraROBs.name()
                      << " because SubDetector " << sid.human_detector() << " is disabled in the detector mask");
    }
  }
  m_extraROBs = validatedExtraROBs;

  // Check SubDets
  std::vector<uint32_t> validatedExtraSubDets;
  for (const uint32_t subdet_id32 : m_extraSubDets.value()) {
    const eformat::SubDetector subdet_id = static_cast<eformat::SubDetector>(subdet_id32 & 0xFF);
    if (detectorMask.is_set(subdet_id)) {
      validatedExtraSubDets.push_back(subdet_id32);
    }
    else {
      ATH_MSG_WARNING("Removing 0x" << MSG::hex << subdet_id << MSG::dec << " from " << m_extraSubDets.name()
                      << " because SubDetector " << eformat::helper::SourceIdentifier(subdet_id,0,0).human_detector()
                      << " is disabled in the detector mask");
    }
  }
  m_extraSubDets = validatedExtraSubDets;

  return StatusCode::SUCCESS;
}
