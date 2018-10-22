/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// Trigger includes
#include "TrigByteStreamCnvSvc.h"

// Athena includes
#include "AthenaKernel/EventContextClid.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "EventInfo/EventInfo.h"
#include "StoreGate/StoreGateSvc.h"

// TDAQ includes
#include "eformat/StreamTag.h"
#include "hltinterface/DataCollector.h"

// System includes
#include <sstream>


// =============================================================================
// Standard constructor
// =============================================================================
TrigByteStreamCnvSvc::TrigByteStreamCnvSvc(const std::string& name, ISvcLocator* svcLoc)
: ByteStreamCnvSvcBase(name, svcLoc),
  m_evtStore("StoreGateSvc", name),
  m_robDataProviderSvc("ROBDataProviderSvc", name) {}

// =============================================================================
// Standard destructor
// =============================================================================
TrigByteStreamCnvSvc::~TrigByteStreamCnvSvc() {}

// =============================================================================
// Implementation of Service::initialize
// =============================================================================
StatusCode TrigByteStreamCnvSvc::initialize() {
  ATH_MSG_VERBOSE("start of " << __FUNCTION__);
  ATH_CHECK(m_evtStore.retrieve());
  ATH_CHECK(m_robDataProviderSvc.retrieve());
  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of Service::finalize
// =============================================================================
StatusCode TrigByteStreamCnvSvc::finalize() {
  ATH_MSG_VERBOSE("start of " << __FUNCTION__);
  if (m_robDataProviderSvc.release().isFailure())
    ATH_MSG_WARNING("Failed to release service " << m_robDataProviderSvc.typeAndName());
  if (m_evtStore.release().isFailure())
    ATH_MSG_WARNING("Failed to release service " << m_evtStore.typeAndName());
  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of IConversionSvc::connectOutput
// The argument outputFile is not used
// =============================================================================
StatusCode TrigByteStreamCnvSvc::connectOutput(const std::string& /*outputFile*/) {
  ATH_MSG_VERBOSE("start of " << __FUNCTION__);

  // Get the EventContext via event store because the interface doesn't allow passing it explicitly as an argument
  // and we don't want to use ThreadLocalContext. Don't use ReadHandle here because it calls ThreadLocalContext if
  // not given a context (which we want to retrieve).
  const EventContext* eventContext = nullptr;
  if (m_evtStore->retrieve(eventContext).isFailure()) {
    ATH_MSG_ERROR("Failed to retrieve EventContext from the event store");
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG("Creating new RawEventWrite for EventContext = " << *eventContext);
  // Create a new RawEventWrite and copy the header from the input RawEvent
  m_rawEventWrite = new RawEventWrite;
  m_rawEventWrite->copy_header( m_robDataProviderSvc->getEvent(*eventContext)->start() );

  ATH_MSG_VERBOSE("Created RawEventWrite pointer = " << m_rawEventWrite);

  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of IConversionSvc::connectOutput
// The arguments are not used, this overload is implemented only for interface compatibility
// =============================================================================
StatusCode TrigByteStreamCnvSvc::connectOutput(const std::string& outputFile, const std::string& /*openMode*/) {
  return connectOutput(outputFile);
}

// =============================================================================
// Implementation of IConversionSvc::commitOutput
// The arguments outputFile and do_commit are not used
// NOTE: In online HLT, m_rawEventWrite is not a full event, it contains only the HLTResult ROBFragments
// =============================================================================
StatusCode TrigByteStreamCnvSvc::commitOutput(const std::string& /*outputFile*/, bool /*do_commit*/) {
  ATH_MSG_VERBOSE("start of " << __FUNCTION__);

  printRawEvent();

  // Serialise the output FullEventFragment
  std::unique_ptr<uint32_t[]> rawEventPtr;
  try {
    const eformat::write::node_t* top = m_rawEventWrite->bind();
    uint32_t rawEventSize = m_rawEventWrite->size_word();
    rawEventPtr = std::make_unique<uint32_t[]>(rawEventSize);
    uint32_t copiedSize = eformat::write::copy(*top,rawEventPtr.get(),rawEventSize);
    if(copiedSize!=rawEventSize) {
      ATH_MSG_ERROR("FullEventFragment serialisation failed");
      return StatusCode::FAILURE;
    }
  }
  catch (const std::exception& e) {
    ATH_MSG_ERROR("FullEventFragment serialisation failed, caught an unexpected std::exception " << e.what());
    return StatusCode::FAILURE;
  }
  catch (...) {
    ATH_MSG_ERROR("FullEventFragment serialisation failed, caught an unexpected exception");
    return StatusCode::FAILURE;
  }

  // Send output to the DataCollector
  try {
    hltinterface::DataCollector::instance()->eventDone(std::move(rawEventPtr));
  }
  catch (const std::exception& e) {
    ATH_MSG_ERROR("Sending output to DataCollector failed, caught an unexpected std::exception " << e.what());
    return StatusCode::FAILURE;
  }
  catch (...) {
    ATH_MSG_ERROR("Sending output to DataCollector failed, caught an unexpected exception");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Serialised FullEventFragment with HLT result was returned to DataCollector successfully");

  delete m_rawEventWrite;
  m_rawEventWrite = nullptr;

  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
  return StatusCode::SUCCESS;
}

// =============================================================================
void TrigByteStreamCnvSvc::printRawEvent() {
  if (!m_rawEventWrite) {
    ATH_MSG_WARNING("RawEventWrite pointer is null");
    return;
  }
  std::ostringstream ss;
  ss << "Dumping header of the FullEventFragment with HLT result:" << std::endl;
  ss << "--> source_id           = 0x" << MSG::hex << m_rawEventWrite->source_id() << MSG::dec << std::endl;
  ss << "--> checksum_type       = 0x" << MSG::hex << m_rawEventWrite->checksum_type() << MSG::dec << std::endl;
  ss << "--> compression_type    = 0x" << MSG::hex << m_rawEventWrite->compression_type() << MSG::dec << std::endl;
  ss << "--> compression_level   = " << m_rawEventWrite->compression_level() << std::endl;
  ss << "--> bc_time_seconds     = " << m_rawEventWrite->bc_time_seconds() << std::endl;
  ss << "--> bc_time_nanoseconds = " << m_rawEventWrite->bc_time_nanoseconds() << std::endl;
  ss << "--> global_id           = " << m_rawEventWrite->global_id() << std::endl;
  ss << "--> run_type            = " << m_rawEventWrite->run_type() << std::endl;
  ss << "--> run_no              = " << m_rawEventWrite->run_no() << std::endl;
  ss << "--> lumi_block          = " << m_rawEventWrite->lumi_block() << std::endl;
  ss << "--> lvl1_id             = " << m_rawEventWrite->lvl1_id() << std::endl;
  ss << "--> bc_id               = " << m_rawEventWrite->bc_id() << std::endl;
  ss << "--> lvl1_trigger_type   = " << (unsigned int)(m_rawEventWrite->lvl1_trigger_type()) << std::endl;
  ss << "--> run_no              = " << m_rawEventWrite->run_no() << std::endl;

  const uint32_t* lvl1_trigger_info = m_rawEventWrite->lvl1_trigger_info();
  ss << "--> lvl1_trigger_info   = ";
  for (size_t i=0; i<m_rawEventWrite->nlvl1_trigger_info(); ++i)
    ss << std::hex << lvl1_trigger_info[i] << std::dec;
  ss << std::endl;

  const uint32_t* lvl2_trigger_info = m_rawEventWrite->lvl2_trigger_info();
  ss << "--> lvl2_trigger_info   = ";
  for (size_t i=0; i<m_rawEventWrite->nlvl2_trigger_info(); ++i)
    ss << std::hex << lvl2_trigger_info[i] << std::dec;
  ss << std::endl;

  const uint32_t* event_filter_info = m_rawEventWrite->event_filter_info();
  ss << "--> event_filter_info   = ";
  for (size_t i=0; i<m_rawEventWrite->nevent_filter_info(); ++i)
    ss << std::hex << event_filter_info[i] << std::dec;
  ss << std::endl;

  const uint32_t* hlt_info = m_rawEventWrite->hlt_info();
  ss << "--> hlt_info            = ";
  for (size_t i=0; i<m_rawEventWrite->nhlt_info(); ++i)
    ss << std::hex << hlt_info[i] << std::dec;
  ss << std::endl;

  std::vector<eformat::helper::StreamTag> stream_tags;
  eformat::helper::decode (m_rawEventWrite->nstream_tag(), m_rawEventWrite->stream_tag(), stream_tags);
  ss << "--> stream_tags         = ";
  bool first = true;
  for (const auto& st : stream_tags) {
    if (first) first=false;
    else ss << "                          ";
    ss << "{" << st.name << ", " << st.type << ", obeysLB=" << st.obeys_lumiblock << ", robs=[";
    for (const auto& robid : st.robs) ss << "0x" << std::hex << robid << std::dec << ", ";
    ss << "], dets = [";
    for (const auto& detid : st.dets) ss << "0x" << std::hex << detid << std::dec << ", ";
    ss << "]}" << std::endl;
  }

  ATH_MSG_INFO(ss.str());
}
