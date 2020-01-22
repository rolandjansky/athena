/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// Trigger includes
#include "TrigByteStreamCnvSvc.h"

// Athena includes
#include "AthenaKernel/EventContextClid.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "TrigKernel/HltPscErrorCode.h"

// Gaudi includes
#include "GaudiKernel/ITHistSvc.h"

// TDAQ includes
#include "eformat/FullEventFragmentNoTemplates.h"
#include "eformat/ROBFragmentNoTemplates.h"
#include "eformat/StreamTag.h"
#include "hltinterface/DataCollector.h"

// System includes
#include <sstream>
#include <iomanip>
#include <chrono>

// Local helper functions
namespace {
  constexpr float wordsToKiloBytes = 0.001*sizeof(uint32_t);
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
  m_robDataProviderSvc("ROBDataProviderSvc", name),
  m_THistSvc("THistSvc", name) {}

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
  ATH_CHECK(m_THistSvc.retrieve());
  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
  return StatusCode::SUCCESS;
}

// =============================================================================
// Reimplementation of Service::start
// =============================================================================
StatusCode TrigByteStreamCnvSvc::start() {
  bookHistograms();
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
  if (m_THistSvc.release().isFailure())
    ATH_MSG_WARNING("Failed to release service " << m_THistSvc.typeAndName());
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

  monitorRawEvent(rawEventPtr);

  // Send output to the DataCollector
  StatusCode result = StatusCode::SUCCESS;
  try {
    auto startTime = std::chrono::high_resolution_clock::now();
    hltinterface::DataCollector::instance()->eventDone(std::move(rawEventPtr));
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    m_histEventDoneTime->Fill(static_cast<float>(duration.count()));
    ATH_MSG_DEBUG("Serialised FullEventFragment with HLT result was returned to DataCollector successfully, "
                  << "the eventDone call took " << duration.count() << " microseconds");
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
void TrigByteStreamCnvSvc::monitorRawEvent(const std::unique_ptr<uint32_t[]>& rawEventPtr) const {
  // Create a read fragment from the pointer
  eformat::read::FullEventFragment rawEvent(rawEventPtr.get());

  // Monitor error code
  if (rawEvent.nstatus() > 1) {
    hltonl::PSCErrorCode errorCode = static_cast<hltonl::PSCErrorCode>(rawEvent.status()[1]);
    std::ostringstream ss;
    ss << errorCode;
    m_histPscErrorCode->Fill(ss.str().data(), 1.0);
  }

  // Decode stream tags
  std::vector<eformat::helper::StreamTag> streamTags;
  try {
    eformat::helper::decode(rawEvent.nstream_tag(), rawEvent.stream_tag(), streamTags);
  }
  catch (const std::exception& ex) {
    ATH_MSG_ERROR("StreamTag decoding failed, caught an unexpected std::exception " << ex.what());
    return;
  }
  catch (...) {
    ATH_MSG_ERROR("StreamTag decoding failed, caught an unexpected exception");
    return;
  }

  // Get HLT result sizes
  std::vector<eformat::read::ROBFragment> robs;
  rawEvent.robs(robs);
  std::unordered_map<uint16_t, uint32_t> resultSizes; // {module ID, size in words}
  uint32_t totalSizeWords = 0;
  try {
    for (const eformat::read::ROBFragment& rob : robs) {
      eformat::helper::SourceIdentifier sid(rob.rob_source_id());
      if (sid.subdetector_id() != eformat::SubDetector::TDAQ_HLT)
        continue;
      uint32_t size = rob.fragment_size_word();
      uint16_t module_id = sid.module_id();
      resultSizes[module_id] = size;
      totalSizeWords += size;
    }
  }
  catch (const std::exception& ex) {
    ATH_MSG_ERROR("HLT result ROB monitoring failed, caught an unexpected std::exception " << ex.what());
    return;
  }
  catch (...) {
    ATH_MSG_ERROR("HLT result ROB monitoring failed, caught an unexpected exception");
    return;
  }

  // Associate result sizes to streams and fill histograms
  m_histStreamTagsNum->Fill(streamTags.size());
  std::unordered_map<std::string, uint32_t> resultSizesByStream;
  for (const eformat::helper::StreamTag& st : streamTags) {
    std::string typeName = st.type + "_" + st.name;
    m_histStreamTags->Fill(typeName.data(), 1.0);
    m_histStreamTagsType->Fill(st.type.data(), 1.0);
    if (st.robs.size() > 0 || st.dets.size() >0) { // PEB stream tag
      m_histPebRobsNum->Fill(st.robs.size());
      m_histPebSubDetsNum->Fill(st.dets.size());
    }

    bool hasHLTSubDet = st.dets.find(eformat::SubDetector::TDAQ_HLT) != st.dets.end();
    bool includeAll = st.robs.empty() && (st.dets.empty() || hasHLTSubDet);
    if (includeAll) {
      resultSizesByStream[typeName] = totalSizeWords;
      continue;
    }
    uint32_t size = 0;
    std::set<std::string> sdFromRobList;
    std::set<std::string> sdFromSubDetList;
    for (const eformat::SubDetector sd : st.dets) {
      sdFromSubDetList.insert(eformat::helper::SubDetectorDictionary.string(sd));
    }
    for (uint32_t robid : st.robs) {
      eformat::helper::SourceIdentifier sid(robid);
      sdFromRobList.insert(sid.human_detector());
      if (sid.subdetector_id() != eformat::SubDetector::TDAQ_HLT)
        continue;
      if (resultSizes.find(sid.module_id()) == resultSizes.end()) {
        ATH_MSG_WARNING("Stream tag " << typeName << " declares " << sid.human()
                        << " in ROB list, but the ROBFragment is missing");
        continue;
      }
      size += resultSizes[sid.module_id()];
    }
    resultSizesByStream[typeName] = size;
    for (const std::string& sdName : sdFromRobList) {
      m_histPebSubDetsFromRobList->Fill(sdName.data(), 1.0);
    }
    for (const std::string& sdName : sdFromSubDetList) {
      m_histPebSubDetsFromSubDetList->Fill(sdName.data(), 1.0);
    }
  }

  // Fill result size and stream tag correlation histograms
  for (const auto& [typeName, size] : resultSizesByStream) {
    m_histResultSizeByStream->Fill(typeName.data(), size*wordsToKiloBytes, 1.0);
    for (const auto& [typeName2, size2] : resultSizesByStream) {
      m_histStreamTagsCorr->Fill(typeName.data(), typeName2.data(), 1.0);
    }
  }
  for (const auto& [moduleId, size] : resultSizes) {
    m_histResultSizeByModule->Fill(static_cast<float>(moduleId), size*wordsToKiloBytes, 1.0);
  }
  m_histResultSizeTotal->Fill(totalSizeWords*wordsToKiloBytes);
  m_histResultSizeFullEvFrag->Fill(rawEvent.fragment_size_word()*wordsToKiloBytes);
  ATH_MSG_DEBUG("Total size of HLT ROBs is " << totalSizeWords*wordsToKiloBytes
                << " kB and FullEventFragment size is " << rawEvent.fragment_size_word()*wordsToKiloBytes << " kB");
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

// =============================================================================
void TrigByteStreamCnvSvc::bookHistograms() {
  const std::string path = "/EXPERT/HLTFramework/" + name() + "/";

  // Function to register histogram in THistSvc (moves the ownership to THistSvc)
  auto regHist = [&path, this](TH1* hist){
    if (m_THistSvc->regHist(path + hist->GetName(), hist).isFailure())
      ATH_MSG_WARNING("Cannot register monitoring histogram " << hist->GetName());
  };

  m_histPscErrorCode = new TH1I(
    "PscErrorCode", "PSC error codes;;Events", 1, 0, 1);
  m_histPscErrorCode->SetCanExtend(TH1::kXaxis);
  regHist(m_histPscErrorCode);

  m_histStreamTags = new TH1F(
    "StreamTags", "Stream Tags produced by HLT;;Events", 1, 0, 1);
  m_histStreamTags->SetCanExtend(TH1::kXaxis);
  regHist(m_histStreamTags);

  m_histStreamTagsCorr = new TH2F(
    "StreamTagsCorr", "Stream Tags (produced by HLT) correlation", 1, 0, 1, 1, 0, 1);
  m_histStreamTagsCorr->SetCanExtend(TH1::kAllAxes);
  regHist(m_histStreamTagsCorr);

  m_histStreamTagsNum = new TH1F(
    "StreamTagsNum", "Number of Stream Tags produced by HLT;Number of Stream Tags;Events", 20, 0, 20);
  regHist(m_histStreamTagsNum);

  m_histStreamTagsType = new TH1F(
    "StreamTagsType", "Type of Stream Tags produced by HLT;;Events", 7, 0, 7);
  m_histStreamTagsType->GetXaxis()->SetBinLabel(1, "physics");
  m_histStreamTagsType->GetXaxis()->SetBinLabel(2, "calibration");
  m_histStreamTagsType->GetXaxis()->SetBinLabel(3, "express");
  m_histStreamTagsType->GetXaxis()->SetBinLabel(4, "monitoring");
  m_histStreamTagsType->GetXaxis()->SetBinLabel(5, "debug");
  m_histStreamTagsType->GetXaxis()->SetBinLabel(6, "reserved");
  m_histStreamTagsType->GetXaxis()->SetBinLabel(7, "unknown");
  regHist(m_histStreamTagsType);

  m_histPebRobsNum = new TH1F(
    "StreamTagsPebRobsNum", "Number of ROBs in PEB stream tags;Number of ROBs;Entries", 200, 0, 200);
  regHist(m_histPebRobsNum);

  m_histPebSubDetsNum = new TH1F(
    "StreamTagsPebSubDetsNum", "Number of SubDetectors in PEB stream tags;Number of SubDetectors;Entries", 100, 0, 100);
  regHist(m_histPebSubDetsNum);

  m_histPebSubDetsFromRobList = new TH1F(
    "StreamTagsPebSubDetsFromRobList", "SubDetectors in PEB stream tags ROB list;;Entries", 1, 0, 1);
  m_histPebSubDetsFromRobList->SetCanExtend(TH1::kXaxis);
  regHist(m_histPebSubDetsFromRobList);

  m_histPebSubDetsFromSubDetList = new TH1F(
    "StreamTagsPebSubDetsFromSubDetList", "SubDetectors in PEB stream tags SubDetector list;;Entries", 1, 0, 1);
  m_histPebSubDetsFromSubDetList->SetCanExtend(TH1::kXaxis);
  regHist(m_histPebSubDetsFromSubDetList);

  m_histResultSizeByModule = new TH2F(
    "ResultSizeByModule", "HLT result size by module;Module ID;Size [kB]", 10, 0, 10, 100, 0, 1000);
  regHist(m_histResultSizeByModule);

  m_histResultSizeByStream = new TH2F(
    "ResultSizeByStream", "HLT result size by stream;;Size [kB]", 1, 0, 1, 100, 0, 1000);
  m_histResultSizeByStream->SetCanExtend(TH1::kXaxis);
  regHist(m_histResultSizeByStream);

  m_histResultSizeTotal = new TH1F(
    "ResultSizeTotal", "HLT result total size (sum of all modules);Size [kB];Events", 100, 0, 1000);
  regHist(m_histResultSizeTotal);

  m_histResultSizeFullEvFrag = new TH1F(
    "ResultSizeFullEvFrag", "HLT output FullEventFragment size;Size [kB];Events", 100, 0, 1000);
  m_histResultSizeFullEvFrag->SetCanExtend(TH1::kXaxis);
  regHist(m_histResultSizeFullEvFrag);

  m_histEventDoneTime = new TH1F(
    "TIME_EventDoneCall", "Time of DataCollector::eventDone calls;Time [us];Events", 1000, 0, 1000);
  regHist(m_histEventDoneTime);
}
