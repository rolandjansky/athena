/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// Trigger includes
#include "TrigByteStreamCnvSvc.h"

// Athena includes
#include "AthenaKernel/EventContextClid.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "StoreGate/StoreGateSvc.h"

// TDAQ includes
#include "eformat/StreamTag.h"
#include "hltinterface/DataCollector.h"

// System includes
#include <sstream>
#include <iomanip>

// Local helper functions
namespace {
  template<typename T> struct printWordHex {
    printWordHex(const T w) : word(w) {}
    T word;
  };
  template<typename T> std::ostream& operator<<(std::ostream& str, const printWordHex<T>& pw) {
    str << "0x" << std::hex << std::setfill('0') << std::setw(2*sizeof(T));
    // Prevent printing char as ASCII character
    if (sizeof(T)==1)
      str << static_cast<int>(pw.word);
    else
      str << pw.word;
    str << std::dec;
    return str;
  }
  template<typename T> struct printNWordsHex {
    printNWordsHex(const size_t n, const T* w, const std::string s=" ") : nwords(n), words(w), sep(s) {}
    size_t nwords;
    const T* words;
    std::string sep;
  };
  template<typename T> std::ostream& operator<<(std::ostream& str, const printNWordsHex<T>& pnw) {
    for (size_t i=0; i<pnw.nwords; ++i) {
      str << printWordHex<T>(pnw.words[i]);
      if (i!=pnw.nwords-1) str << pnw.sep;
    }
    return str;
  }
}

// =============================================================================
// Standard constructor
// =============================================================================
TrigByteStreamCnvSvc::TrigByteStreamCnvSvc(const std::string& name, ISvcLocator* svcLoc)
: ByteStreamCnvSvc(name, svcLoc),
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
  ATH_CHECK(ByteStreamCnvSvc::initialize());
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
  ATH_CHECK(ByteStreamCnvSvc::finalize());
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
  const uint32_t* inputRawEvent = m_robDataProviderSvc->getEvent(*eventContext)->start();
  if (!inputRawEvent) {
    ATH_MSG_ERROR("Input RawEvent is nullptr, cannot create output");
    return StatusCode::FAILURE;
  }
  m_rawEventWrite->copy_header(inputRawEvent);

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

  if (msgLvl(MSG::DEBUG)) printRawEvent();

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
  StatusCode result = StatusCode::SUCCESS;
  try {
    hltinterface::DataCollector::instance()->eventDone(std::move(rawEventPtr));
    ATH_MSG_DEBUG("Serialised FullEventFragment with HLT result was returned to DataCollector successfully");
  }
  catch (const std::exception& e) {
    ATH_MSG_ERROR("Sending output to DataCollector failed, caught an unexpected std::exception " << e.what());
    result = StatusCode::FAILURE;
  }
  catch (...) {
    ATH_MSG_ERROR("Sending output to DataCollector failed, caught an unexpected exception");
    result = StatusCode::FAILURE;
  }

  delete m_rawEventWrite;
  m_rawEventWrite = nullptr;

  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
  return result;
}

// =============================================================================
void TrigByteStreamCnvSvc::printRawEvent() {
  if (!m_rawEventWrite) {
    ATH_MSG_WARNING("RawEventWrite pointer is null");
    return;
  }
  std::ostringstream ss;
  ss << "Dumping header of the FullEventFragment with HLT result:" << std::endl;
  ss << "--> status              = "
     << printNWordsHex<uint32_t>(m_rawEventWrite->nstatus(), m_rawEventWrite->status())
     << std::endl;
  ss << "--> source_id           = " << printWordHex<uint32_t>(m_rawEventWrite->source_id()) << std::endl;
  ss << "--> checksum_type       = " << printWordHex<uint32_t>(m_rawEventWrite->checksum_type()) << std::endl;
  ss << "--> compression_type    = " << printWordHex<uint32_t>(m_rawEventWrite->compression_type()) << std::endl;
  ss << "--> compression_level   = " << m_rawEventWrite->compression_level() << std::endl;
  ss << "--> bc_time_seconds     = " << m_rawEventWrite->bc_time_seconds() << std::endl;
  ss << "--> bc_time_nanoseconds = " << m_rawEventWrite->bc_time_nanoseconds() << std::endl;
  ss << "--> global_id           = " << m_rawEventWrite->global_id() << std::endl;
  ss << "--> run_type            = " << m_rawEventWrite->run_type() << std::endl;
  ss << "--> run_no              = " << m_rawEventWrite->run_no() << std::endl;
  ss << "--> lumi_block          = " << m_rawEventWrite->lumi_block() << std::endl;
  ss << "--> lvl1_id             = " << m_rawEventWrite->lvl1_id() << std::endl;
  ss << "--> bc_id               = " << m_rawEventWrite->bc_id() << std::endl;
  ss << "--> lvl1_trigger_type   = " << printWordHex<uint8_t>(m_rawEventWrite->lvl1_trigger_type()) << std::endl;
  ss << "--> lvl1_trigger_info   = "
     << printNWordsHex<uint32_t>(m_rawEventWrite->nlvl1_trigger_info(), m_rawEventWrite->lvl1_trigger_info())
     << std::endl;
  ss << "--> lvl2_trigger_info   = "
     << printNWordsHex<uint32_t>(m_rawEventWrite->nlvl2_trigger_info(), m_rawEventWrite->lvl2_trigger_info())
     << std::endl;
  ss << "--> event_filter_info   = "
     << printNWordsHex<uint32_t>(m_rawEventWrite->nevent_filter_info(), m_rawEventWrite->event_filter_info())
     << std::endl;
  ss << "--> hlt_info            = "
     << printNWordsHex<uint32_t>(m_rawEventWrite->nhlt_info(), m_rawEventWrite->hlt_info())
     << std::endl;

  std::vector<eformat::helper::StreamTag> stream_tags;
  try {
    eformat::helper::decode(m_rawEventWrite->nstream_tag(), m_rawEventWrite->stream_tag(), stream_tags);
  }
  catch (const std::exception& ex) {
    ATH_MSG_ERROR("StreamTag decoding failed, caught an unexpected std::exception " << ex.what());
    return;
  }
  catch (...) {
    ATH_MSG_ERROR("StreamTag decoding failed, caught an unexpected exception");
    return;
  }
  ss << "--> stream_tags         = ";
  bool first = true;
  for (const auto& st : stream_tags) {
    if (first) first=false;
    else ss << "                          ";
    ss << "{" << st.name << ", " << st.type << ", obeysLB=" << st.obeys_lumiblock << ", robs=[";
    for (const auto& robid : st.robs) ss << printWordHex<uint32_t>(robid) << ", ";
    ss << "], dets = [";
    for (const auto& detid : st.dets) ss << printWordHex<uint8_t>(detid) << ", ";
    ss << "]}" << std::endl;
  }

  ATH_MSG_DEBUG(ss.str());
}
