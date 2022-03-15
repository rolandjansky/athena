/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// Local includes
#include "MuonRoIByteStreamTool.h"

// Trigger includes
#include "TrigT1MuctpiBits/HelpersPhase1.h"
#include "xAODTrigger/MuonRoI.h"
#include "xAODTrigger/MuonRoIAuxContainer.h"

// Athena includes
#include "CxxUtils/span.h"

// TDAQ includes
#include "eformat/SourceIdentifier.h"

using ROBF = OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;
using WROBF = OFFLINE_FRAGMENTS_NAMESPACE_WRITE::ROBFragment;

namespace {
  /// x-axis values for word type count histogram
  constexpr static std::array<size_t,static_cast<size_t>(LVL1::MuCTPIBits::WordType::MAX)> s_wordTypes = {0,1,2,3,4,5};
  using namespace std::literals::string_view_literals;
  /// Labels to build sector-specific eta/phi monitored variable name
  constexpr static std::array<std::string_view,4> s_sectorNames = {{"Undefined"sv, "Barrel"sv, "Forward"sv, "Endcap"sv}};
  /// Number of BCIDs in full LHC orbit
  constexpr static int s_bcidsFullOrbit{3564};
  /// BCID diff wrapped into the range [-s_bcidsFullOrbit/2, s_bcidsFullOrbit/2)
  constexpr int bcidDiff(int a, int b) {
    int diff = a - b;
    while (diff < -s_bcidsFullOrbit/2) {diff += s_bcidsFullOrbit;}
    while (diff > s_bcidsFullOrbit/2 - 1) {diff -= s_bcidsFullOrbit;}
    return diff;
  }
  /// BCID sum wrapped into the range [0, s_bcidsFullOrbit)
  constexpr int bcidSum(int a, int b) {
    int sum = a + b;
    while (sum < 0) {sum += s_bcidsFullOrbit;}
    while (sum >= s_bcidsFullOrbit) {sum -= s_bcidsFullOrbit;}
    return sum;
  }
}

MuonRoIByteStreamTool::MuonRoIByteStreamTool(const std::string& type,
                                             const std::string& name,
                                             const IInterface* parent)
: base_class(type, name, parent) {}

StatusCode MuonRoIByteStreamTool::initialize() {
  ConversionMode mode = getConversionMode(m_roiReadKeys, m_roiWriteKeys, msg());
  ATH_CHECK(mode!=ConversionMode::Undefined);
  ATH_CHECK(m_roiWriteKeys.initialize(mode==ConversionMode::Decoding));
  ATH_CHECK(m_roiReadKeys.initialize(mode==ConversionMode::Encoding));

  ATH_MSG_DEBUG((mode==ConversionMode::Encoding ? "Encoding" : "Decoding") << " ROB IDs: "
                << MSG::hex << m_robIds.value() << MSG::dec);

  m_readoutWindow = mode==ConversionMode::Decoding ? m_roiWriteKeys.size() : m_roiReadKeys.size();
  if (m_readoutWindow!=1 && m_readoutWindow!=3 && m_readoutWindow!=5) {
    ATH_MSG_ERROR("The expected readout window must be 1, 3 or 5, but it is " << m_readoutWindow);
    return StatusCode::FAILURE;
  }

  if (m_robIds.value().size() != 1) {
    ATH_MSG_ERROR("This tool implementation assumes there is exactly one MUCTPI ROB, but "
                  << m_robIds.size() << " were configured");
    return StatusCode::SUCCESS;  
  }

  CHECK( m_rpcTool.retrieve() );
  CHECK( m_tgcTool.retrieve() );
  CHECK( m_thresholdTool.retrieve() );
  if (!m_monTool.empty()) ATH_CHECK(m_monTool.retrieve());

  return StatusCode::SUCCESS;
}

// BS->xAOD conversion
StatusCode MuonRoIByteStreamTool::convertFromBS(const std::vector<const ROBF*>& vrobf,
                                                const EventContext& eventContext) const {
  // Create and record the RoI containers
  std::vector<SG::WriteHandle<xAOD::MuonRoIContainer>> handles = m_roiWriteKeys.makeHandles(eventContext);
  for (auto& handle : handles) {
    ATH_CHECK(handle.record(std::make_unique<xAOD::MuonRoIContainer>(),
                            std::make_unique<xAOD::MuonRoIAuxContainer>()));
    ATH_MSG_DEBUG("Recorded MuonRoIContainer with key " << handle.key());
  }

  // Find the ROB fragment to decode
  const eformat::helper::SourceIdentifier sid(m_robIds.value().at(0));
  auto it = std::find_if(vrobf.begin(), vrobf.end(), [&sid](const ROBF* rob){return rob->rob_source_id() == sid.code();});
  if (it == vrobf.end()) {
    ATH_MSG_DEBUG("No MUCTPI ROB fragment with ID " << sid.code() << " was found, MuonRoIContainer will be empty");
    return StatusCode::SUCCESS;
  }

  // Iterate over ROD words and decode
  const ROBF* rob = *it;
  ATH_MSG_DEBUG("MUCTPI ROB for BCID " << rob->rod_bc_id());
  const uint32_t ndata = rob->rod_ndata();
  const uint32_t* const data = rob->rod_data();
  ATH_MSG_DEBUG("Starting to decode " << ndata << " ROD words");

  Monitored::Scalar<uint32_t> monNumWords{"NumWordsInROD", ndata};
  std::array<size_t,static_cast<size_t>(LVL1::MuCTPIBits::WordType::MAX)> wordTypeCounts{}; // zero-initialised
  auto monWordTypeCount = Monitored::Collection("WordTypeCount", wordTypeCounts);
  auto monWordType = Monitored::Collection("WordType", s_wordTypes);
  std::vector<uint32_t> bcidOffsetsWrtROB; // diffs between BCID in timeslice header and BCID in ROB header
  auto monBCIDOffsetsWrtROB = Monitored::Collection("BCIDOffsetsWrtROB", bcidOffsetsWrtROB);

  // We don't know the readout window size at this point, so will collect start and end of roi words for each time
  // slice and decode them later directly into the right time slice output container when we know which slice is which
  std::vector<std::pair<size_t,size_t>> roiSlices; // v of {start, length}

  size_t iWord{0};
  for (const uint32_t word : CxxUtils::span{data, ndata}) {
    ATH_MSG_DEBUG("MUCTPI raw word " << iWord << ": 0x" << std::hex << word << std::dec);
    LVL1::MuCTPIBits::WordType wordType = LVL1::MuCTPIBits::getWordType(word);
    ++wordTypeCounts[static_cast<size_t>(wordType)];

    switch (wordType) {
      case LVL1::MuCTPIBits::WordType::Timeslice: {
        const auto header = LVL1::MuCTPIBits::timesliceHeader(word);
        ATH_MSG_DEBUG("This is a timeslice header word with BCID=" << header.bcid
                      << ", NTOB=" << header.tobCount << ", NCAND=" << header.candCount);
        // create new RoI words slice
        roiSlices.emplace_back(0,0);
        // monitor BCID offset
        bcidOffsetsWrtROB.push_back(bcidDiff(header.bcid, rob->rod_bc_id()));
        break;
      }
      case LVL1::MuCTPIBits::WordType::Multiplicity: {
        uint32_t tmNum = LVL1::MuCTPIBits::multiplicityWordNumber(word);
        ATH_MSG_DEBUG("This is a multiplicity word #" << tmNum);
        break;
      }
      case LVL1::MuCTPIBits::WordType::Candidate: {
        ATH_MSG_DEBUG("This is a RoI candidate word");
        // advance slice edges
        std::pair<size_t,size_t>& slice = roiSlices.back();
        if (slice.first==0) slice.first = iWord;
        slice.second = iWord - slice.first + 1;
        break;
      }
      case LVL1::MuCTPIBits::WordType::Topo: {
        ATH_MSG_DEBUG("This is a Topo TOB word");
        break;
      }
      case LVL1::MuCTPIBits::WordType::Status: {
        ATH_MSG_DEBUG("This is a status word");
        std::vector<size_t> errorBits = LVL1::MuCTPIBits::getDataStatusWordErrors(word);
        // TODO: Decide on the action in case of errors, ATR-25069
        if (!errorBits.empty()) {
          ATH_MSG_DEBUG("MUCTPI ROD data flagged with errors. The data status word is 0x" << std::hex << word << std::dec);
          for (size_t bit : errorBits) {
            ATH_MSG_DEBUG("Error bit " << bit << ": " << LVL1::MuCTPIBits::DataStatusWordErrors.at(bit));
          }
          auto monErrorBits = Monitored::Collection("DataStatusWordErrors", errorBits);
          Monitored::Group(m_monTool, monErrorBits);
        }
        break;
      }
      default: {
        ATH_MSG_ERROR("The MUCTPI word 0x" << std::hex << word << std::dec << " does not match any known word type");
        return StatusCode::FAILURE;
      }
    }
    ++iWord;
  } // Loop over all ROD words

  // Now decode the RoI candidate words in each time slice
  const size_t nSlices{roiSlices.size()};
  const size_t nOutputSlices{static_cast<size_t>(m_readoutWindow)};
  if (nSlices > nOutputSlices) {
    ATH_MSG_ERROR("Found " << nSlices << " time slices, but only " << m_readoutWindow << " outputs are configured");
    return StatusCode::FAILURE;
  } else if (nSlices!=1 && nSlices!=3 && nSlices!=5) {
    ATH_MSG_ERROR("Expected 1, 3 or 5 time slices but found " << nSlices);
    return StatusCode::FAILURE;
  }
  const size_t outputOffset = nOutputSlices/2 - nSlices/2;
  auto outputIt = handles.begin();
  std::advance(outputIt, outputOffset);
  for (const auto& [sliceStart,sliceSize] : roiSlices) {
    for (const uint32_t onlineWord : CxxUtils::span{data+sliceStart, sliceSize}) {
      ATH_MSG_DEBUG("Decoding RoI word 0x" << std::hex << onlineWord << std::dec << " into the " << outputIt->key() << " container");
      uint32_t offlineWord = LVL1::MuCTPIBits::convertSectorIDOnlineToOffline(onlineWord);
      ATH_MSG_DEBUG("Converted the RoI word to offline format: 0x" << std::hex << offlineWord << std::dec);

      // Create a new xAOD::MuonRoI object for this candidate in the output container
      (*outputIt)->push_back(std::make_unique<xAOD::MuonRoI>());

      // Decode eta/phi information using the right tool for the subsystem
      LVL1::TrigT1MuonRecRoiData roiData;
      const LVL1::MuCTPIBits::SubsysID subsysID = LVL1::MuCTPIBits::getSubsysID(onlineWord, /*onlineFormat=*/ true);
      switch (subsysID) {
        case LVL1::MuCTPIBits::SubsysID::Endcap: // same for Endcap and Forward
        case LVL1::MuCTPIBits::SubsysID::Forward: {
          ATH_MSG_DEBUG("This is an Endcap/Forward candidate, calling the " << m_tgcTool.typeAndName());
          roiData = m_tgcTool->roiData(offlineWord);
          break;
        }
        case LVL1::MuCTPIBits::SubsysID::Barrel: {
          ATH_MSG_DEBUG("This is a Barrel candidate, calling the " << m_rpcTool.typeAndName());
          roiData = m_rpcTool->roiData(offlineWord);
          break;
        }
        default: {
          ATH_MSG_ERROR("Failed to determine Sector ID from RoI word 0x" << std::hex << onlineWord << std::dec);
          return StatusCode::FAILURE;
        }
      }

      // Get the threshold decisions to find the lowest pt threshold passed
      // This is required by xAOD::MuonRoI::initialize() but not used for HLT seeding (a threshold pattern bit mask is used instead)
      const std::pair<std::string, double> minThrInfo = m_thresholdTool->getMinThresholdNameAndValue(
        m_thresholdTool->getThresholdDecisions(offlineWord, eventContext),
        roiData.eta());

      // Fill the xAOD::MuonRoI object
      (*outputIt)->back()->initialize((offlineWord | 0x1u<<31), // Flag Run-3 format in the unused MSB
                                      roiData.eta(),
                                      roiData.phi(),
                                      minThrInfo.first,
                                      minThrInfo.second);

      // Fill per-candidate monitoring histograms
      using SubsysID_ut = std::underlying_type_t<LVL1::MuCTPIBits::SubsysID>;
      Monitored::Scalar<SubsysID_ut> monSubsysID{"SubsysID", static_cast<SubsysID_ut>(subsysID)};
      std::string sectorName{s_sectorNames[static_cast<size_t>(subsysID)]};
      Monitored::Scalar<double> monEta{"roiEta_"+sectorName, roiData.eta()};
      Monitored::Scalar<double> monPhi{"roiPhi_"+sectorName, roiData.phi()};
      Monitored::Group(m_monTool, monSubsysID, monEta, monPhi);
    }
    ++outputIt;
  } // Loop over RoI candidate time slices

  // Output monitoring
  short bcOffset{static_cast<short>(5/2 - m_readoutWindow/2 - 2)};
  for (auto& handle : handles) {
    ATH_MSG_DEBUG("Decoded " << handle->size() << " Muon RoIs into the " << handle.key() << " container");
    Monitored::Scalar<short> monBCOffset{"BCOffset", bcOffset};
    Monitored::Scalar<size_t> monNumRoIs{"NumOutputRoIs", handle->size()};
    Monitored::Group(m_monTool, monBCOffset, monNumRoIs);
    ++bcOffset;
  }

  // Fill per-event monitoring histograms
  Monitored::Group(m_monTool, monNumWords, monWordType, monWordTypeCount, monBCIDOffsetsWrtROB);

  return StatusCode::SUCCESS;
}

/// xAOD->BS conversion
StatusCode MuonRoIByteStreamTool::convertToBS(std::vector<WROBF*>& vrobf,
                                              const EventContext& eventContext) {
  // Retrieve the RoI containers and determine how many time slices will be encoded
  std::vector<SG::ReadHandle<xAOD::MuonRoIContainer>> handles = m_roiReadKeys.makeHandles(eventContext);
  int nSlices{0};
  int iHandle{0};
  size_t rodSize{0};
  for (auto& handle : handles) {
    ATH_CHECK(handle.isValid());
    if (!handle->empty()) {
      rodSize += handle->size();
      nSlices = std::max(nSlices, 1+2*std::abs(m_readoutWindow/2 - iHandle));
    }
    ++iHandle;
  }
  if (nSlices==0) {
    ATH_MSG_DEBUG("There are no muon RoIs to encode in this event");
    // Force at least the triggered BC slice to be encoded
    nSlices = 1;
  }

  // Calculate the number of words we need to allocate (candidate words already counted above)
  rodSize += 4*nSlices + 1; // 1 timeslice header per slice, 3 multiplicity words per slice, 1 status word
  ATH_MSG_DEBUG("Going to encode " << nSlices << " time slices into " << rodSize << " ROD words");

  // Clear BS data cache and allocate new data array
  clearCache(eventContext);
  uint32_t* data = newRodData(eventContext, rodSize); // Owned by the cache

  // Initialise a few monitoring variables
  Monitored::Scalar<size_t> monNumWords{"NumWordsInROD", rodSize};
  std::array<size_t,static_cast<size_t>(LVL1::MuCTPIBits::WordType::MAX)> wordTypeCounts{}; // zero-initialised
  auto monWordTypeCount = Monitored::Collection("WordTypeCount", wordTypeCounts);
  auto monWordType = Monitored::Collection("WordType", s_wordTypes);
  std::vector<uint32_t> bcidOffsetsWrtROB; // diffs between BCID in timeslice header and BCID in ROB header
  auto monBCIDOffsetsWrtROB = Monitored::Collection("BCIDOffsetsWrtROB", bcidOffsetsWrtROB);
  auto monitorCandidate = [](const auto& monTool, const xAOD::MuonRoI& roi){
      // Fill per-candidate monitoring histograms
      const uint32_t word = roi.roiWord();
      using SubsysID_ut = std::underlying_type_t<LVL1::MuCTPIBits::SubsysID>;
      const LVL1::MuCTPIBits::SubsysID subsysID = LVL1::MuCTPIBits::getSubsysID(word, /*onlineFormat=*/ false);
      Monitored::Scalar<SubsysID_ut> monSubsysID{"SubsysID", static_cast<SubsysID_ut>(subsysID)};
      std::string sectorName{s_sectorNames[static_cast<size_t>(subsysID)]};
      Monitored::Scalar<double> monEta{"roiEta_"+sectorName, roi.eta()};
      Monitored::Scalar<double> monPhi{"roiPhi_"+sectorName, roi.phi()};
      Monitored::Group(monTool, monSubsysID, monEta, monPhi);
  };

  // Fill the data words
  auto inputIt = handles.begin();
  std::advance(inputIt, m_readoutWindow/2 - nSlices/2);
  size_t iWord{0};
  for (int iSlice=0; iSlice<nSlices; ++iSlice, ++inputIt) {
    // Timeslice header
    uint32_t bcid = bcidSum(eventContext.eventID().bunch_crossing_id(), iSlice - nSlices/2);
    static constexpr uint32_t tobCount = 0; // Filling Topo words not implemented
    uint32_t candCount = (*inputIt)->size();
    data[iWord++] = LVL1::MuCTPIBits::timesliceHeader(bcid, tobCount, candCount);
    ++wordTypeCounts[static_cast<size_t>(LVL1::MuCTPIBits::WordType::Timeslice)];
    bcidOffsetsWrtROB.push_back(bcidDiff(bcid, eventContext.eventID().bunch_crossing_id()));
    ATH_MSG_DEBUG("Added timeslice header word with BCID=" << bcid << ", NTOB=" << tobCount << ", NCAND=" << candCount);

    // Multiplicity words
    std::array<uint32_t,3> multiplicityWords = LVL1::MuCTPIBits::multiplicityWords(0, 0, false); // Multiplicity words content not implemented
    for (const uint32_t word : multiplicityWords) {
      data[iWord++] = word;
    }
    wordTypeCounts[static_cast<size_t>(LVL1::MuCTPIBits::WordType::Multiplicity)] += multiplicityWords.size();
    ATH_MSG_DEBUG("Added " << multiplicityWords.size() << " multiplicity words");

    // Candidate words
    for (const xAOD::MuonRoI* roi : **inputIt) {
      monitorCandidate(m_monTool, *roi);
      data[iWord++] = LVL1::MuCTPIBits::convertSectorIDOfflineToOnline(roi->roiWord());
      ATH_MSG_DEBUG("Added RoI word 0x" << std::hex << roi->roiWord() << std::dec
                    << " converted to online format: 0x" << std::hex << data[iWord-1] << std::dec);
    }
    wordTypeCounts[static_cast<size_t>(LVL1::MuCTPIBits::WordType::Candidate)] += (*inputIt)->size();
    ATH_MSG_DEBUG("Added " << (*inputIt)->size() << " candidate words");
  }

  // Status word
  data[iWord++] = LVL1::MuCTPIBits::dataStatusWord(0);
  ++wordTypeCounts[static_cast<size_t>(LVL1::MuCTPIBits::WordType::Status)];
  ATH_MSG_DEBUG("Added the data status word");

  // Check that we filled all words
  if (iWord!=rodSize) {
    ATH_MSG_ERROR("Expected to fill " << rodSize << " ROD words but filled " << iWord);
    return StatusCode::FAILURE;
  }

  // Create a ROBFragment containing the ROD words
  const eformat::helper::SourceIdentifier sid(m_robIds.value().at(0));
  vrobf.push_back(newRobFragment(eventContext, sid.code(), rodSize, data));

  // Fill per-event monitoring histograms
  Monitored::Group(m_monTool, monNumWords, monWordType, monWordTypeCount, monBCIDOffsetsWrtROB);

  return StatusCode::SUCCESS;
}
