/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Local includes
#include "L1TopoLegacyOnlineMonitor.h"

// Trigger includes
#include "L1TopoRDO/BlockTypes.h"
#include "L1TopoRDO/Helpers.h"
#include "L1TopoRDO/Fibre.h"
#include "L1TopoRDO/Status.h"
#include "TrigT1Result/CTP_Decoder.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"

// Athena includes
#include "AthenaKernel/SlotSpecificObj.h" // for SG::getNSlots()

// TDAQ and external includes
#include "eformat/SourceIdentifier.h"
#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/Ranlux64Engine.h"

// System includes
#include <utility>

// Typedefs
using ROBF = OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;

// Local helper constants and functions
namespace {
  /// String for debug printout of TOB bits information
  std::string tobBitInfoString(unsigned int sourceID,
                               const L1Topo::L1TopoTOB& tob,
                               unsigned int bit,
                               unsigned int index) {
    std::ostringstream ss;
    ss << "filling";
    ss << " index " << index << " with";
    ss << " value " << ((tob.trigger_bits()>>bit)&1);
    ss << " sourceID " << L1Topo::formatHex8(sourceID);
    ss << " tob: idx clk fpga";
    ss << " " << tob.index();
    ss << " " << tob.clock();
    ss << " " << tob.fpga();
    ss << " bit " << bit;
    return ss.str();
  }
  /// Return a vector of indices corresponding to bits set to 1 in the bitset
  template<size_t N>
  std::vector<size_t> bitsetIndices(const std::bitset<N>& bits) {
    std::vector<size_t> indices;
    indices.reserve(bits.count());
    for (size_t i=0; i<bits.size(); ++i) {
      if (bits[i]) indices.push_back(i);
    }
    return indices;
  }
}

// =============================================================================
// Standard constructor
// =============================================================================
L1TopoLegacyOnlineMonitor::L1TopoLegacyOnlineMonitor(const std::string& name, ISvcLocator* svcLoc)
: AthReentrantAlgorithm(name, svcLoc),
  m_RNGEngines([](){return static_cast<CLHEP::HepRandomEngine*>(new CLHEP::Ranlux64Engine);}, SG::getNSlots()) {}

// =============================================================================
// Implementation of AthReentrantAlgorithm::initialize
// =============================================================================
StatusCode L1TopoLegacyOnlineMonitor::initialize() {
  const bool daqAccessEnabled = m_prescaleDaqRobAccess.value() >= 1;

  ATH_CHECK(m_errorFlagsKey.initialize());
  ATH_CHECK(m_RoIBResultKey.initialize(m_doCnvMon.value()));
  ATH_CHECK(m_daqRdoCollectionKey.initialize(m_doCnvMon.value() && daqAccessEnabled));
  ATH_CHECK(m_simTopoCTPKey.initialize(m_doSimMon.value()));
  ATH_CHECK(m_simTopoOverflowCTPKey.initialize(m_doSimMon.value()));
  ATH_CHECK(m_ctpRdoKey.initialize(m_doSimMon.value() && daqAccessEnabled));
  ATH_CHECK(m_robDataProviderSvc.retrieve());
  ATH_CHECK(m_monTool.retrieve(DisableTool{m_monTool.name().empty()}));

  auto topoRobId = [](const uint16_t moduleId) {
    return eformat::helper::SourceIdentifier{eformat::TDAQ_CALO_TOPO_PROC, moduleId}.code();
  };
  for (const uint16_t moduleId : m_roibRobModuleIds) {
    m_roibRobIds.push_back(topoRobId(moduleId));
  }
  for (const uint16_t moduleId : m_daqRobModuleIds) {
    m_daqRobIds.push_back(topoRobId(moduleId));
  }

  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of AthReentrantAlgorithm::execute
// =============================================================================
StatusCode L1TopoLegacyOnlineMonitor::execute(const EventContext& eventContext) const {
  // Create and record the ErrorFlags container
  SG::WriteHandle<xAOD::TrigCompositeContainer> errorFlagsCont(m_errorFlagsKey, eventContext);
  ATH_CHECK(errorFlagsCont.record(std::make_unique<xAOD::TrigCompositeContainer>(),
                              std::make_unique<xAOD::TrigCompositeAuxContainer>()));
  ATH_MSG_DEBUG("Recorded TrigCompositeContainer with key " << m_errorFlagsKey.key());

  // Create the ErrorFlags object
  errorFlagsCont->push_back(new xAOD::TrigComposite);
  xAOD::TrigComposite& errorFlags = *(errorFlagsCont->back());
  resetFlags(errorFlags);

  // Calculate DAQ ROB access prescale decision
  bool allowDAQROBAccess{false};
  if (m_prescaleDaqRobAccess.value() == 1) {
    allowDAQROBAccess = true;
  }
  else if (m_prescaleDaqRobAccess.value() > 1) {
    // Implementation following PrescalingTool from the L1Decoder package
    size_t seed = eventContext.eventID().time_stamp() ^ eventContext.eventID().time_stamp_ns_offset();
    CLHEP::HepRandomEngine* engine = m_RNGEngines.getEngine(eventContext);
    engine->setSeed( seed, 0 );
    auto flat = engine->flat();
    allowDAQROBAccess = flat < 1./ m_prescaleDaqRobAccess.value();
    if (!allowDAQROBAccess) ATH_MSG_DEBUG("DAQ ROB access is prescaled for this event");
  }

  DecisionBits decisionBits{};
  enum class MonFunction : uint8_t {doRawMon=0, doCnvMon, doSimMon, doOverflowSimMon, doSimDaq};
  std::vector<uint8_t> failedMonFunctions;

  if (m_doRawMon) {
    StatusCode sc = doRawMon(allowDAQROBAccess, eventContext);
    ATH_MSG_DEBUG("Executed doRawMon: " << (sc.isFailure() ? "failed" : "ok"));
    if (sc.isFailure()) {
      failedMonFunctions.push_back(static_cast<uint8_t>(MonFunction::doRawMon));
    }
  }

  if (m_doCnvMon) {
    StatusCode sc = doCnvMon(errorFlags, decisionBits, allowDAQROBAccess, eventContext);
    ATH_MSG_DEBUG("Executed doCnvMon: " << (sc.isFailure() ? "failed" : "ok"));
    if (sc.isFailure()) {
      failedMonFunctions.push_back(static_cast<uint8_t>(MonFunction::doCnvMon));
    }
  }

  if (m_doSimMon) {
    StatusCode sc = doSimMon(errorFlags, decisionBits, allowDAQROBAccess, eventContext);
    ATH_MSG_DEBUG("Executed doSimMon: " << (sc.isFailure() ? "failed" : "ok"));
    if (sc.isFailure()) {
      failedMonFunctions.push_back(static_cast<uint8_t>(MonFunction::doSimMon));
    }
    sc = doOverflowSimMon(decisionBits, eventContext);
    ATH_MSG_DEBUG("Executed doOverflowSimMon: " << (sc.isFailure() ? "failed" : "ok"));
    if (sc.isFailure()) {
      failedMonFunctions.push_back(static_cast<uint8_t>(MonFunction::doOverflowSimMon));
    }
  }

  if (m_doSimDaq && allowDAQROBAccess) {
    StatusCode sc = doSimDaq(decisionBits);
    ATH_MSG_DEBUG("Executed doSimDaq: " << (sc.isFailure() ? "failed" : "ok"));
    if (sc.isFailure()) {
      failedMonFunctions.push_back(static_cast<uint8_t>(MonFunction::doSimDaq));
    }
  }

  auto monFailedMonFunctions = Monitored::Collection("MonitoringFailures", failedMonFunctions);
  Monitored::Group(m_monTool, monFailedMonFunctions);

  return StatusCode::SUCCESS;
}

// =============================================================================

StatusCode L1TopoLegacyOnlineMonitor::doRawMon(bool allowDAQROBAccess, const EventContext& eventContext) const {
  IROBDataProviderSvc::VROBFRAG roibRobs;
  m_robDataProviderSvc->getROBData(eventContext, m_roibRobIds, roibRobs, name());
  ATH_MSG_DEBUG("RawMon: received " << roibRobs.size() << " RoIB ROBs");
  ATH_CHECK(monitorROBs(roibRobs, true));

  if (allowDAQROBAccess) {
    IROBDataProviderSvc::VROBFRAG daqRobs;
    m_robDataProviderSvc->getROBData(eventContext, m_daqRobIds, daqRobs, name());
    ATH_MSG_DEBUG("RawMon: received " << daqRobs.size() << " DAQ ROBs");
    ATH_CHECK(monitorROBs(daqRobs, false));
  }

  return StatusCode::SUCCESS;
}

// =============================================================================

StatusCode L1TopoLegacyOnlineMonitor::doCnvMon(xAOD::TrigComposite& errorFlags,
                                               DecisionBits& decisionBits,
                                               bool allowDAQROBAccess,
                                               const EventContext& eventContext) const {
  SG::ReadHandle<ROIB::RoIBResult> roibResult{m_RoIBResultKey, eventContext};
  if (!roibResult.isValid()) {
    ATH_MSG_DEBUG("Failed to retrieve RoIBResult object with key " << m_RoIBResultKey.key());
    return StatusCode::FAILURE;
  }

  std::vector<std::reference_wrapper<Monitored::IMonitoredVariable>> allMonVars;
  std::vector<uint8_t> problems;
  std::vector<L1Topo::L1TopoTOB> roiTobs;
  std::vector<size_t> tobCrcErrors; // TOB CRC errors to monitor
  std::vector<uint32_t> sourceIDs; // All seen source IDs to monitor
  /// Trigger bits sent to CTP
  std::bitset<s_nTopoCTPOutputs>& triggerBits = DecisionBits::createBits(decisionBits.triggerBits);
  /// Overflow bits corresponding to CTP output
  std::bitset<s_nTopoCTPOutputs>& overflowBits = DecisionBits::createBits(decisionBits.overflowBits);

  // Get the RoI results
  const std::vector<ROIB::L1TopoResult>& l1TopoResults = roibResult->l1TopoResult();
  ATH_MSG_DEBUG("Number of L1Topo ROI RODs found: "<<l1TopoResults.size());
  if (l1TopoResults.empty()) {
    problems.push_back(static_cast<uint8_t>(Problems::ROI_NO_RDO));
  }

  // Loop over RoI results
  for (const ROIB::L1TopoResult& r : l1TopoResults) {
    // Get the RDO
    const L1TopoRDO& rdo = r.rdo();
    ATH_MSG_DEBUG("Found RoI RDO with source ID " << L1Topo::formatHex8(rdo.getSourceID()));
    sourceIDs.push_back(rdo.getSourceID());

    // Check for RDO errors
    const std::vector<L1Topo::Error>& errors = rdo.getErrors();
    if (!rdo.getErrors().empty()) {
      ATH_MSG_INFO("RoI Converter errors reported: " << errors);
      problems.push_back(static_cast<uint8_t>(Problems::ROI_CNV_ERR));
      errorFlags.setDetail("hasGenericRoiError", true);
    }

    // Get the RDO data words
    const std::vector<uint32_t>& dataWords = rdo.getDataWords();
    if (dataWords.empty()) {
      ATH_MSG_INFO("L1TopoRDO RoI payload is empty");
      problems.push_back(static_cast<uint8_t>(Problems::ROI_PAYLOAD_EMPTY));
      errorFlags.setDetail("hasGenericRoiError", true);
    }

    // Loop over RDO data words
    for (const uint32_t word : dataWords) {
      ATH_MSG_VERBOSE("Got RoI word: " << L1Topo::formatHex8(word));
      // Check TOB type
      if (L1Topo::blockType(word) != L1Topo::BlockTypes::L1TOPO_TOB) {
        ATH_MSG_WARNING("Unexpected TOB type in RoI: " << L1Topo::formatHex8(word));
        problems.push_back(static_cast<uint8_t>(Problems::ROI_BAD_TOB));
        errorFlags.setDetail("hasGenericRoiError", true);
      }

      // Collect the TOB and check for CRC errors
      roiTobs.emplace_back(word);
      const L1Topo::L1TopoTOB& tob = roiTobs.back();
      ATH_MSG_DEBUG(tob);
      size_t bin{0};
      for (bool crc : {tob.crc_EM(), tob.crc_Tau(), tob.crc_Muon(), tob.crc_Jet(), tob.crc_Energy()}) {
        if (crc){
          tobCrcErrors.push_back(bin);
          errorFlags.setDetail("hasCrcTobError", true);
        }
        ++bin;
      }

      // Collect trigger and overflow bits in bitsets
      for (unsigned int iBit=0; iBit<8; ++iBit){
        unsigned int index = L1Topo::triggerBitIndexNew(rdo.getSourceID(), tob, iBit);
        ATH_MSG_VERBOSE(tobBitInfoString(rdo.getSourceID(), tob, iBit, index));
        triggerBits[index] = (tob.trigger_bits()>>iBit)&1;
        overflowBits[index] = (tob.overflow_bits()>>iBit)&1;
      }
    } // End of loop over RDO data words
  } // End of loop over RoI results

  std::vector<size_t> triggerBitIndices = bitsetIndices(triggerBits);
  std::vector<size_t> overflowBitIndices = bitsetIndices(overflowBits);
  ATH_MSG_VERBOSE("Trigger bits from RoI Cnv: " << triggerBitIndices);
  ATH_MSG_VERBOSE("Overflow bits from RoI Cnv: " << overflowBitIndices);

  auto monProblems = Monitored::Collection("Problems", problems);
  auto monCTPSigPartRoICnv = Monitored::Collection("CTP_sig_part_fromROICnv", roiTobs,
                                                   &L1Topo::L1TopoTOB::ctp_signal);
  auto monCrcErrors = Monitored::Collection("InputLinkCRCs", tobCrcErrors);
  auto monSourceIDs = Monitored::Collection("ROB_src_IDs_fromCnv", sourceIDs,
                                            [](const uint32_t sid){return L1Topo::formatHex8(sid);});
  auto monTriggerBits = Monitored::Collection("CTP_Trigger_fromROICnv", triggerBitIndices);
  auto monOverflowBits = Monitored::Collection("CTP_Overflow_fromROICnv", overflowBitIndices);
  allMonVars.insert(allMonVars.end(), {monCTPSigPartRoICnv, monCrcErrors, monSourceIDs,
                                       monTriggerBits, monOverflowBits, monProblems});

  if (!allowDAQROBAccess) {
    // DAQ access prescaled in this event, nothing more to monitor
    // - fill the histograms and return
    Monitored::Group(m_monTool, std::move(allMonVars));
    return StatusCode::SUCCESS;
  }

  // Collect BC0 DAQ TOBs to monitor trigger and overflow bits and compare to ROIB
  std::vector<std::pair<uint32_t, L1Topo::L1TopoTOB>> daqTobsBC0; // vector of {sourceID, tob} pairs

  // Get the DAQ results
  SG::ReadHandle<L1TopoRDOCollection> daqRdoCollection{m_daqRdoCollectionKey, eventContext};
  if (!daqRdoCollection.isValid()) {
    problems.push_back(static_cast<uint8_t>(Problems::DAQ_NO_RDO));
    errorFlags.setDetail("hasGenericDaqError", true);
    // Fill the histograms and return
    Monitored::Group(m_monTool, std::move(allMonVars));
    return StatusCode::SUCCESS;
  }
  if (daqRdoCollection->empty()) {
    problems.push_back(static_cast<uint8_t>(Problems::DAQ_COLL_EMPTY));
    errorFlags.setDetail("hasGenericDaqError", true);
    // Fill the histograms and return
    Monitored::Group(m_monTool, std::move(allMonVars));
    return StatusCode::SUCCESS;
  }

  // Loop over DAQ results
  for (const L1TopoRDO* rdo : *daqRdoCollection) {
    ATH_MSG_VERBOSE(*rdo);
    ATH_MSG_DEBUG("Found DAQ RDO with source ID " << L1Topo::formatHex8(rdo->getSourceID()));
    sourceIDs.push_back(rdo->getSourceID());

    // Check for RDO errors
    const std::vector<L1Topo::Error>& errors = rdo->getErrors();
    if (!errors.empty()) {
      ATH_MSG_INFO("DAQ Converter errors reported: " << errors);
      problems.push_back(static_cast<uint8_t>(Problems::DAQ_CNV_ERR));
      errorFlags.setDetail("hasGenericDaqError", true);
    }

    // Get the RDO data words
    const std::vector<uint32_t>& dataWords = rdo->getDataWords();
    if (dataWords.empty()) {
      ATH_MSG_INFO("L1TopoRDO DAQ payload is empty");
      problems.push_back(static_cast<uint8_t>(Problems::DAQ_PAYLOAD_EMPTY));
      errorFlags.setDetail("hasGenericDaqError", true);
    }

    // Loop over RDO data words
    L1TopoBlock block{};
    bool firstWord{true};
    for (const uint32_t word : dataWords) {
      switch (L1Topo::blockType(word)) {
        case L1Topo::BlockTypes::HEADER: {
          // New block detected, monitor the one just completed
          if (!firstWord) {monitorBlock(rdo->getSourceID(), block, errorFlags);}
          // Reset the block data
          block = L1TopoBlock{};
          block.header = L1Topo::Header(word);
          break;
        }
        case L1Topo::BlockTypes::FIBRE: {
          L1Topo::Fibre fibreBlock{word};
          block.fibreSizes.insert(block.fibreSizes.end(),
                                  fibreBlock.count().begin(),
                                  fibreBlock.count().end());
          block.fibreStatus.insert(block.fibreStatus.end(),
                                   fibreBlock.status().begin(),
                                   fibreBlock.status().end());
          break;
        }
        case L1Topo::BlockTypes::STATUS: {
          L1Topo::Status status{word};
          ATH_MSG_DEBUG("CnvMon: fibre overflow: " << status.overflow() << " fibre crc: " << status.crc());
          if (status.overflow()) {
            problems.push_back(static_cast<uint8_t>(Problems::FIBRE_OVERFLOW));
          }
          if (status.crc()) {
            problems.push_back(static_cast<uint8_t>(Problems::FIBRE_CRC));
            errorFlags.setDetail("hasCrcFibreError", true);
          }
          break;
        }
        case L1Topo::BlockTypes::L1TOPO_TOB: {
          block.daqTobs.emplace_back(static_cast<int32_t>(word)); // TODO: Fix L1TopoTOB ctor to take uint32_t word
          if (block.header.bcn_offset() == 0) {
            daqTobsBC0.emplace_back(rdo->getSourceID(), block.daqTobs.back());
          }
          break;
        }
        default: {
          break;
        }
      }
      firstWord = false;
    } // End of loop over RDO data words
    // Monitor the last block
    monitorBlock(rdo->getSourceID(), block, errorFlags);
  } // End of loop over DAQ results

  // Compare ROI and DAQ L1Topo TOBS
  if (roiTobs.empty() and daqTobsBC0.empty()) {
    ATH_MSG_DEBUG("L1Topo TOBs from both ROI and DAQ via converters are empty: zero supression or problem?");
    errorFlags.setDetail("hasGenericRoiError", true);
    errorFlags.setDetail("hasGenericDaqError", true);
  }

  // The comparison of all words is non-trivial, because of different ordering and zero suppression
  // (some TOBs could be missing from the DAQ output). So we just compare the decision bits.
  ATH_MSG_DEBUG("Assigning trigger and overflow bitsets from " << daqTobsBC0.size() << " words from the DAQ ROB");
  /// Trigger bits from DAQ ROB
  std::bitset<s_nTopoCTPOutputs>& triggerBitsDaqRob = DecisionBits::createBits(decisionBits.triggerBitsDaqRob);
  /// Overflow bits from DAQ ROB
  std::bitset<s_nTopoCTPOutputs>& overflowBitsDaqRob = DecisionBits::createBits(decisionBits.overflowBitsDaqRob);
  for (const auto& [sourceID, tob] : daqTobsBC0) {
    for(unsigned int iBit=0; iBit<8; ++iBit){
      unsigned int index = L1Topo::triggerBitIndexNew(sourceID, tob, iBit);
      triggerBitsDaqRob[index] = (tob.trigger_bits()>>iBit)&1;
      overflowBitsDaqRob[index] = (tob.overflow_bits()>>iBit)&1;
    }
  }
  std::vector<size_t> triggerBitIndicesDaqRob = bitsetIndices(triggerBitsDaqRob);
  std::vector<size_t> overflowBitIndicesDaqRob = bitsetIndices(overflowBitsDaqRob);
  ATH_MSG_VERBOSE("Trigger bits from DAQ ROB Cnv: " << triggerBitIndicesDaqRob);
  ATH_MSG_VERBOSE("Overflow bits from DAQ ROB Cnv: " << overflowBitIndicesDaqRob);

  const std::vector<size_t> triggerBitsDiff = compBitSets("L1Topo hardware", "L1Topo DAQ ROB",
                                                          triggerBits, triggerBitsDaqRob);
  auto monTriggerBitsDiff = Monitored::Collection("DAQ_ROB_Hdw_vs_L1Topo_Hdw_Events", triggerBitsDiff);
  if (!triggerBitsDiff.empty()) {
    errorFlags.setDetail("hasRoibDaqDifference", true);
    allMonVars.push_back(monTriggerBitsDiff);
  }

  // Fill the histograms
  Monitored::Group(m_monTool, std::move(allMonVars));
  return StatusCode::SUCCESS;
}

// =============================================================================

StatusCode L1TopoLegacyOnlineMonitor::doSimMon(xAOD::TrigComposite& errorFlags,
                                               DecisionBits& decisionBits,
                                               bool allowDAQROBAccess,
                                               const EventContext& eventContext) const {
  // Retrieve the L1Topo simulation result
  SG::ReadHandle<LVL1::FrontPanelCTP> simTopoCTP{m_simTopoCTPKey, eventContext};
  if (!simTopoCTP.isValid()) {
    ATH_MSG_DEBUG("Failed to retrieve LVL1::FrontPanelCTP object with key \"" << m_simTopoCTPKey.key()
                  << "\". Skipping simulation comparison");
    return StatusCode::FAILURE;
  }
  // Check hardware results from RoIB are available (from doCnvMon)
  if (!decisionBits.triggerBits.has_value() || !decisionBits.overflowBits.has_value()) {
    ATH_MSG_DEBUG("Trigger and/or overflow bits not set. Skipping simulation comparison");
    return StatusCode::FAILURE;
  }

  std::vector<std::reference_wrapper<Monitored::IMonitoredVariable>> allMonVars;
  std::bitset<s_nTopoCTPOutputs>& triggerBits = decisionBits.triggerBits.value(); // Alias
  std::bitset<s_nTopoCTPOutputs>& overflowBits = decisionBits.overflowBits.value(); // Alias

  // Fill a bitset with simulation result
  std::bitset<s_nTopoCTPOutputs>& triggerBitsSim = DecisionBits::createBits(decisionBits.triggerBitsSim);
  for (uint32_t iBit=0; iBit<32; ++iBit) {
    uint32_t mask = (1u << iBit);
    if ((simTopoCTP->cableWord1(0) & mask) != 0) {
      // cable 1, clock 0
      triggerBitsSim[iBit] = true;
    }
    if ((simTopoCTP->cableWord1(1) & mask) != 0) {
      // cable 1, clock 1
      triggerBitsSim[32 + iBit] = true;
    }
    if ((simTopoCTP->cableWord2(0) & mask) != 0) {
      // cable 2, clock 0
      triggerBitsSim[64 + iBit] = true;
    }
    if ((simTopoCTP->cableWord2(1) & mask) != 0) {
      // cable 2, clock 1
      triggerBitsSim[96 + iBit] = true;
    }
  }

  // Do the comparison and fill histograms only if the L1Topo items did not overflow
  if (overflowBits.none()) {
    const std::vector<size_t> triggerBitIndicesSim = bitsetIndices(triggerBitsSim);
    const std::vector<size_t> triggerBitIndicesHdw = bitsetIndices(triggerBits);
    const std::vector<size_t> triggerBitIndicesSimNotHdw = bitsetIndices(triggerBitsSim & (~triggerBits));
    const std::vector<size_t> triggerBitIndicesHdwNotSim = bitsetIndices(triggerBits & (~triggerBitsSim));
    auto monSim = Monitored::Collection("SimResults", triggerBitIndicesSim);
    auto monHdw = Monitored::Collection("HdwResults", triggerBitIndicesHdw);
    auto monSimNotHdw = Monitored::Collection("SimNotHdwResult", triggerBitIndicesSimNotHdw);
    auto monHdwNotSim = Monitored::Collection("HdwNotSimResult", triggerBitIndicesHdwNotSim);
    Monitored::Group(m_monTool, monSim, monHdw, monSimNotHdw, monHdwNotSim);

    // debug printout
    ATH_MSG_DEBUG("Simulated output from L1Topo from StoreGate with key " << m_simTopoCTPKey.key());
    ATH_MSG_DEBUG("L1Topo word 1 at clock 0 is: 0x" << L1Topo::formatHex8(simTopoCTP->cableWord1(0)));
    ATH_MSG_DEBUG("L1Topo word 2 at clock 0 is: 0x" << L1Topo::formatHex8(simTopoCTP->cableWord2(0)));
    ATH_MSG_DEBUG("L1Topo word 1 at clock 1 is: 0x" << L1Topo::formatHex8(simTopoCTP->cableWord1(1)));
    ATH_MSG_DEBUG("L1Topo word 2 at clock 1 is: 0x" << L1Topo::formatHex8(simTopoCTP->cableWord2(1)));
  }
  // Perform the hardware versus simulation comparison
  const std::vector<size_t> simHdwDiff = compBitSets("L1Topo hardware", "L1Topo simulation",
                                                     triggerBits, triggerBitsSim);
  auto monSimHdwDiff = Monitored::Collection("Hdw_vs_Sim_Events", simHdwDiff);
  allMonVars.push_back(monSimHdwDiff);

  if (!allowDAQROBAccess) {
    // DAQ access prescaled in this event, nothing more to monitor
    // - fill the histograms and return
    Monitored::Group(m_monTool, std::move(allMonVars));
    return StatusCode::SUCCESS;
  }

  // Retrieve CTP DAQ data for comparison
  SG::ReadHandle<CTP_RDO> ctpRdo{m_ctpRdoKey, eventContext};
  if (!ctpRdo.isValid()) {
    ATH_MSG_DEBUG("Failed to retrieve CTP_RDO object (converted from CTP DAQ ROB) with key \""
                  << m_ctpRdoKey.key() << "\". Skipping CTP hardware comparison");
    // Fill the histograms and return
    Monitored::Group(m_monTool, std::move(allMonVars));
    return StatusCode::FAILURE;
  }

  // CTP RDO contains 17 TIP words for a number of BCs, so use CTP_Decoder to access accepted BC
  CTP_Decoder ctp;
  ctp.setRDO(ctpRdo.cptr());
  const uint32_t l1aPos = ctpRdo->getL1AcceptBunchPosition();
  if (l1aPos >= ctp.getBunchCrossings().size()) {
    ATH_MSG_DEBUG("CTP_RDO gave invalid l1aPos. Skipping CTP hardware comparison");
    // Fill the histograms and return
    Monitored::Group(m_monTool, std::move(allMonVars));
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("CTP l1aPos, size: " << l1aPos << ", " << ctp.getBunchCrossings().size());
  const CTP_BC& ctpL1a = ctp.getBunchCrossings().at(l1aPos);

  // Fill decision bits from CTP RDO
  std::bitset<s_nTopoCTPOutputs>& triggerBitsCtp = DecisionBits::createBits(decisionBits.triggerBitsCtp);
  static constexpr size_t ctpTIPSize{512};
  const std::bitset<ctpTIPSize>& tip = ctpL1a.getTIP();
  ATH_MSG_VERBOSE("CTP TIP bits: " << tip.to_string());
  static constexpr size_t topoTipStart{ctpTIPSize - s_nTopoCTPOutputs}; // TODO: read L1Topo TIP position from TriggerDB
  for (size_t i=0; i<s_nTopoCTPOutputs; ++i) {
    triggerBitsCtp[i] = tip[i+topoTipStart];
  }

  // Compare hardware bits from L1Topo RoIB and CTP
  const std::vector<size_t> hdwCtpBitsDiff = compBitSets("L1Topo hardware trigger|overflow", "CTP TIP hardware",
                                                         triggerBits|overflowBits, triggerBitsCtp);
  auto monHdwCtpBitsDiff = Monitored::Collection("CTP_Hdw_vs_L1Topo_Hdw_Events", hdwCtpBitsDiff);
  if (!hdwCtpBitsDiff.empty()) {
    errorFlags.setDetail("hasRoibCtpDifference", true);
    allMonVars.push_back(monHdwCtpBitsDiff);
  }

  // Compare bits from CTP hardware and L1Topo simulation
  if (overflowBits.none()) {
    const std::vector<size_t> ctpSimHdwBitsDiff = compBitSets("L1Topo CTP TIP hardware", "L1Topo CTP simulation",
                                                              triggerBitsCtp, triggerBitsSim);
    auto monCtpSimHdwBitsDiff = Monitored::Collection("CTP_Hdw_vs_Sim_Events", ctpSimHdwBitsDiff);
    if (!ctpSimHdwBitsDiff.empty()) {
      allMonVars.push_back(monCtpSimHdwBitsDiff);
    }
  }

  // Compare hardware bits from L1Topo DAQ and CTP
  // Check hardware results from RoIB are available (from doCnvMon)
  if (!decisionBits.triggerBitsDaqRob.has_value() || !decisionBits.overflowBitsDaqRob.has_value()) {
    ATH_MSG_DEBUG("Trigger and/or overflow bits from DAQ ROB not set. Skipping DAQ to CTP comparison");
    // Fill the histograms and return
    Monitored::Group(m_monTool, std::move(allMonVars));
    return StatusCode::FAILURE;
  }
  std::bitset<s_nTopoCTPOutputs>& triggerBitsDaqRob = decisionBits.triggerBitsDaqRob.value(); // Alias
  std::bitset<s_nTopoCTPOutputs>& overflowBitsDaqRob = decisionBits.overflowBitsDaqRob.value(); // Alias
  const std::vector<size_t> daqRobCtpBitsDiff = compBitSets("L1Topo DAQ ROB trigger|overflow", "CTP TIP hardware",
                                                            triggerBitsDaqRob|overflowBitsDaqRob, triggerBitsCtp);
  auto monDaqRobCtpBitsDiff = Monitored::Collection("DAQ_ROB_Hdw_vs_CTP_Hdw_Events", daqRobCtpBitsDiff);
  if (!daqRobCtpBitsDiff.empty()) {
    errorFlags.setDetail("hasDaqCtpDifference", true);
    allMonVars.push_back(monDaqRobCtpBitsDiff);
  }

  // Fill the histograms
  Monitored::Group(m_monTool, std::move(allMonVars));
  return StatusCode::SUCCESS;
}

// =============================================================================

StatusCode L1TopoLegacyOnlineMonitor::doOverflowSimMon(DecisionBits& decisionBits,
                                                       const EventContext& eventContext) const {
  // Retrieve the L1Topo simulation overflow result
  SG::ReadHandle<LVL1::FrontPanelCTP> simTopoOverflowCTP{m_simTopoOverflowCTPKey, eventContext};
  if (!simTopoOverflowCTP.isValid()) {
    ATH_MSG_DEBUG("Failed to retrieve LVL1::FrontPanelCTP object with key \"" << m_simTopoOverflowCTPKey.key()
                  << "\". Skipping simulation overflow comparison");
    return StatusCode::FAILURE;
  }
  // Check hardware results from RoIB are available (from doCnvMon)
  if (!decisionBits.overflowBits.has_value()) {
    ATH_MSG_DEBUG("Overflow bits not set. Skipping simulation comparison");
    return StatusCode::FAILURE;
  }
  std::bitset<s_nTopoCTPOutputs>& overflowBits = decisionBits.overflowBits.value(); // Alias

  // Fill a bitset with simulation overflow result
  std::bitset<s_nTopoCTPOutputs>& overflowBitsSim = DecisionBits::createBits(decisionBits.overflowBitsSim);
  for (uint32_t iBit=0; iBit<32; ++iBit) {
    uint32_t mask = (1u << iBit);
    if ((simTopoOverflowCTP->cableWord1(0) & mask) != 0) {
      // cable 1, clock 0
      overflowBitsSim[iBit] = true;
    }
    if ((simTopoOverflowCTP->cableWord1(1) & mask) != 0) {
      // cable 1, clock 1
      overflowBitsSim[32 + iBit] = true;
    }
    if ((simTopoOverflowCTP->cableWord2(0) & mask) != 0) {
      // cable 2, clock 0
      overflowBitsSim[64 + iBit] = true;
    }
    if ((simTopoOverflowCTP->cableWord2(1) & mask) != 0) {
      // cable 2, clock 1
      overflowBitsSim[96 + iBit] = true;
    }
  }

  // Fill the histograms
  const std::vector<size_t> overflowBitIndicesSim = bitsetIndices(overflowBitsSim);
  const std::vector<size_t> overflowBitIndicesHdw = bitsetIndices(overflowBits);
  const std::vector<size_t> overflowBitIndicesSimNotHdw = bitsetIndices(overflowBitsSim & (~overflowBits));
  const std::vector<size_t> overflowBitIndicesHdwNotSim = bitsetIndices(overflowBits & (~overflowBitsSim));
  auto monSim = Monitored::Collection("SimOverflows", overflowBitIndicesSim);
  auto monHdw = Monitored::Collection("HdwOverflows", overflowBitIndicesHdw);
  auto monSimNotHdw = Monitored::Collection("SimNotHdwOverflow", overflowBitIndicesSimNotHdw);
  auto monHdwNotSim = Monitored::Collection("HdwNotSimOverflow", overflowBitIndicesHdwNotSim);

  // debug printout
  ATH_MSG_DEBUG("Simulated overflow from L1Topo from StoreGate with key " << m_simTopoOverflowCTPKey.key());
  ATH_MSG_DEBUG("L1Topo overflow word 1 at clock 0 is: 0x" << L1Topo::formatHex8(simTopoOverflowCTP->cableWord1(0)));
  ATH_MSG_DEBUG("L1Topo overflow word 2 at clock 0 is: 0x" << L1Topo::formatHex8(simTopoOverflowCTP->cableWord2(0)));
  ATH_MSG_DEBUG("L1Topo overflow word 1 at clock 1 is: 0x" << L1Topo::formatHex8(simTopoOverflowCTP->cableWord1(1)));
  ATH_MSG_DEBUG("L1Topo overflow word 2 at clock 1 is: 0x" << L1Topo::formatHex8(simTopoOverflowCTP->cableWord2(1)));

  // Perform the hardware versus simulation comparison
  const std::vector<size_t> simHdwDiff = compBitSets("L1Topo hardware overflow", "L1Topo simulated overflow",
                                                     overflowBits, overflowBitsSim);
  auto monSimHdwDiff = Monitored::Collection("Hdw_vs_Sim_EventOverflow", simHdwDiff);

  Monitored::Group(m_monTool, monSim, monHdw, monSimNotHdw, monHdwNotSim, monSimHdwDiff);
  return StatusCode::SUCCESS;
}

// =============================================================================

StatusCode L1TopoLegacyOnlineMonitor::doSimDaq(DecisionBits& decisionBits) const {
  if (!decisionBits.triggerBitsSim.has_value() or
      !decisionBits.triggerBitsDaqRob.has_value() or
      !decisionBits.overflowBitsDaqRob.has_value()) {
    ATH_MSG_DEBUG("Simulation and/or DAQ ROB bits not set. Skipping simulation to DAQ comparison");
    return StatusCode::FAILURE;
  }
  std::bitset<s_nTopoCTPOutputs>& triggerBitsSim = decisionBits.triggerBitsSim.value(); // Alias
  std::bitset<s_nTopoCTPOutputs>& triggerBitsDaqRob = decisionBits.triggerBitsDaqRob.value(); // Alias
  std::bitset<s_nTopoCTPOutputs>& overflowBitsDaqRob = decisionBits.overflowBitsDaqRob.value(); // Alias

  if (overflowBitsDaqRob.any()) {
    // Skip if there are overflows in DAQ ROB
    return StatusCode::SUCCESS;
  }

  std::vector<size_t> triggerBitIndicesSim = bitsetIndices(triggerBitsSim);
  std::vector<size_t> triggerBitIndicesDaq = bitsetIndices(triggerBitsDaqRob);
  std::vector<size_t> triggerBitIndicesSimNotDaq = bitsetIndices(triggerBitsSim & (~triggerBitsDaqRob));
  std::vector<size_t> triggerBitIndicesDaqNotSim = bitsetIndices(triggerBitsDaqRob & (~triggerBitsSim));
  auto monSim = Monitored::Collection("SimDaqRobResults", triggerBitIndicesSim);
  auto monDaq = Monitored::Collection("HdwDaqRobResults", triggerBitIndicesDaq);
  auto monSimNotDaq = Monitored::Collection("SimNotHdwDaqRobResult", triggerBitIndicesSimNotDaq);
  auto monDaqNotSim = Monitored::Collection("HdwNotSimDaqRobResult", triggerBitIndicesDaqNotSim);

  Monitored::Group(m_monTool, monSim, monDaq, monSimNotDaq, monDaqNotSim);
  return StatusCode::SUCCESS;
}

// =============================================================================

StatusCode L1TopoLegacyOnlineMonitor::monitorROBs(
    const std::vector<const ROBF*>& robs,
    bool isROIB) const {
  // Loop over ROBs
  for (const ROBF* rob : robs) {
    // Monitor ROB ID
    auto monRobId = Monitored::Scalar<std::string>("ROB_src_IDs_fromROB", L1Topo::formatHex8(rob->source_id()));
    ATH_MSG_DEBUG("RawMon: found ROB ID " << monRobId.getString(0));

    // Loop over data words
    const uint32_t ndata = rob->rod_ndata();
    const uint32_t* data = rob->rod_data();
    std::vector<uint8_t> blockTypes{};
    ATH_MSG_DEBUG("RawMon: found " << ndata << " ROD words in this ROB");
    for (uint32_t i=0; i<ndata; ++i, ++data) {
      ATH_MSG_VERBOSE("-- " << L1Topo::formatHex8(*data));
      blockTypes.push_back(static_cast<uint8_t>(L1Topo::blockType(*data)));
    }
    std::string sourceLabel = isROIB ? "ROI" : "DAQ";
    auto monBlockTypes = Monitored::Collection(std::string("TOBtype_from")+sourceLabel+"ROB", blockTypes);
    auto monPayloadSize = Monitored::Scalar<uint32_t>(sourceLabel+"_ROB_payload_size", ndata);

    // Fill the histograms
    Monitored::Group(m_monTool, monRobId, monBlockTypes, monPayloadSize);
  }
  return StatusCode::SUCCESS;
}

// =============================================================================

void L1TopoLegacyOnlineMonitor::monitorBlock(const uint32_t sourceID,
                                             const L1TopoBlock& block,
                                             xAOD::TrigComposite& errorFlags) const {
  ATH_MSG_DEBUG("monitorBlock:");
  ATH_MSG_DEBUG(block.header);
  std::vector<std::reference_wrapper<Monitored::IMonitoredVariable>> allMonVars;
  Monitored::Scalar<uint32_t> monPayloadCRC{"CRC_fromDAQCnv", std::numeric_limits<uint32_t>::max()};
  if (block.header.payload_crc() != 0) {
    ATH_MSG_INFO("Header payload CRC not zero: " << L1Topo::formatHex8(block.header.payload_crc()));
    errorFlags.setDetail("hasCrcDaqError", true);
    monPayloadCRC = block.header.payload_crc();
    allMonVars.push_back(monPayloadCRC);
  }
  auto monFibreStatus = Monitored::Collection("NonZero_FibreStatus_fromDAQCnv", block.fibreStatus);
  allMonVars.push_back(monFibreStatus);
  if (msgLvl(MSG::DEBUG)) {
    size_t iFibre{0};
    for (const uint32_t status : block.fibreStatus) {
      if (status != 0) {
        ATH_MSG_DEBUG("monitorBlock warning: Fibre status set for fibre " << iFibre
                      << " of ROB " << L1Topo::formatHex8(sourceID) << " header " << block.header);
      }
      ++iFibre;
    }
  }
  // BCNs
  auto monBCN = Monitored::Scalar("DAQ_ROB_rel_bx_fromCnv", block.header.bcn());
  allMonVars.push_back(monBCN);

  // Fill the histograms
  Monitored::Group(m_monTool, std::move(allMonVars));
}

// =============================================================================

std::vector<size_t> L1TopoLegacyOnlineMonitor::compBitSets(const std::string& leftLabel,
                                                           const std::string& rightLabel,
                                                           const std::bitset<s_nTopoCTPOutputs>& left,
                                                           const std::bitset<s_nTopoCTPOutputs>& right) const {
  if (left == right) {
    ATH_MSG_DEBUG("compBitSets: " << leftLabel << " and " << rightLabel << " are the same");
    return {};
  }
  std::bitset<s_nTopoCTPOutputs> diff = (left ^ right);
  std::vector<size_t> diffIndices = bitsetIndices(diff);
  ATH_MSG_DEBUG("compBitSets: " << leftLabel << " and " << rightLabel << " differ in this event:\n"
                << left << "\n" << right);
  ATH_MSG_VERBOSE("Indices of differing bits:" << diffIndices);
  return diffIndices;
}

// =============================================================================

void L1TopoLegacyOnlineMonitor::resetFlags(xAOD::TrigComposite& errorFlags) const {
  errorFlags.setDetail("hasGenericRoiError", false);
  errorFlags.setDetail("hasGenericDaqError", false);
  errorFlags.setDetail("hasCrcTobError", false);
  errorFlags.setDetail("hasCrcFibreError", false);
  errorFlags.setDetail("hasCrcDaqError", false);
  errorFlags.setDetail("hasRoibDaqDifference", false);
  errorFlags.setDetail("hasRoibCtpDifference", false);
  errorFlags.setDetail("hasDaqCtpDifference", false);
}
