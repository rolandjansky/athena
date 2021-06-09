/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1TOPOONLINEMONITORING_L1TopoLegacyOnlineMonitor_h
#define L1TOPOONLINEMONITORING_L1TopoLegacyOnlineMonitor_h

// Trigger includes
#include "L1TopoRDO/L1TopoRDOCollection.h"
#include "L1TopoRDO/Header.h"
#include "L1TopoRDO/L1TopoTOB.h"
#include "TrigT1Interfaces/FrontPanelCTP.h"
#include "TrigT1Interfaces/TrigT1StoreGateKeys.h"
#include "TrigT1Result/CTP_RDO.h"
#include "TrigT1Result/RoIBResult.h"
#include "xAODTrigger/TrigComposite.h"
#include "xAODTrigger/TrigCompositeContainer.h"

// Athena includes
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaKernel/RNGWrapper.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

// System includes
#include <bitset>
#include <optional>

/**
 *  @class L1TopoLegacyOnlineMonitor
 *  @brief Algorithm to monitor L1Topo transmission and simulation errors for the legacy (Run-2) L1Topo boards
 *  @author Davide Gerbaudo
 *  @author Simon George
 *  @author Rafal Bielski
 *
 *  This algorithm fetches L1Topo data from different sources, and
 *  performs comparisons trying to detect errors.
 *  The input collections are read in from:
 *  - ROI path: ROIB::RoIBResult. Read out full L1Topo RDO (TOBs, CRC,
 *    trigger+overflow bits) every event, w/out zero suppression.
 *  - DAQ path: L1TopoRDOCollection. Read out zero-suppressed TOBs and
 *    trigger bits every N events.
 *  - Real-time path to CTP: CTP_RDO. Read out trigger bits from the
 *    TIP bus of the CTP. Note that these bits are OR'ed with the
 *    overflow ones.
 *  - Simulation: LVL1::FrontPanelCTP. Trigger and overflow bits from
 *    L1TopoSimulation.
 *
 *  The results of the comparisons are saved in a xAOD::TrigComposite object
 *  and can be later used to accept the event for offline investigation
 **/
class L1TopoLegacyOnlineMonitor : public AthReentrantAlgorithm {
public:
  L1TopoLegacyOnlineMonitor(const std::string& name, ISvcLocator* svcLoc);

  // ------------------------- AthReentrantAlgorithm methods -------------------
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& eventContext) const override;

private:
  // ------------------------- Private types and constants ---------------------
  /// Number of CTP outputs, used for histogram ranges and loops
  static constexpr size_t s_nTopoCTPOutputs{128};
  /// Enum used to assign an integer ID and histogram bin to various types of problem that arise and should be reported
  enum class Problems : uint8_t {
    ROI_NO_RDO=0, ROI_CNV_ERR, ROI_PAYLOAD_EMPTY, ROI_BAD_TOB,
    DAQ_NO_RDO, DAQ_COLL_EMPTY, DAQ_CNV_ERR, DAQ_PAYLOAD_EMPTY, DAQ_BAD_TOB,
    FIBRE_OVERFLOW, FIBRE_CRC
  };
  /// Helper structure representing L1Topo block, passed to the \c monitorBlock method
  struct L1TopoBlock {
    // default-initialise with version 15, BCN -7, which is unlikely, to avoid misinterpreting default as a valid header
    L1Topo::Header header{0xf,0,0,0,0,1,0x7};
    std::vector<uint32_t> fibreSizes;
    std::vector<uint32_t> fibreStatus;
    std::vector<L1Topo::L1TopoTOB> daqTobs;
  };
  /// Helper structure holding decision bitsets, passed between the monitoring methods
  struct DecisionBits {
    std::optional<std::bitset<s_nTopoCTPOutputs>> triggerBits;
    std::optional<std::bitset<s_nTopoCTPOutputs>> overflowBits;
    std::optional<std::bitset<s_nTopoCTPOutputs>> triggerBitsDaqRob;
    std::optional<std::bitset<s_nTopoCTPOutputs>> overflowBitsDaqRob;
    std::optional<std::bitset<s_nTopoCTPOutputs>> triggerBitsSim;
    std::optional<std::bitset<s_nTopoCTPOutputs>> overflowBitsSim;
    std::optional<std::bitset<s_nTopoCTPOutputs>> triggerBitsCtp;
    /// Helper method to create a new bitset and get a reference to it
    static std::bitset<s_nTopoCTPOutputs>& createBits(std::optional<std::bitset<s_nTopoCTPOutputs>>& opt) {
      opt = std::bitset<s_nTopoCTPOutputs>{};
      return opt.value();
    }
  };

  // ------------------------- Properties and handles --------------------------
  SG::WriteHandleKey<xAOD::TrigCompositeContainer> m_errorFlagsKey {
    this, "ErrorFlagsKey", "L1TopoErrorFlags_Legacy",
    "Key of the output TrigCompositeContainer with L1Topo error flags"};
  SG::ReadHandleKey<ROIB::RoIBResult> m_RoIBResultKey {
    this, "RoIBResult", "RoIBResult",
    "Key of the RoIBResult object holding decoded L1Topo information from RoIBuilder"};
  SG::ReadHandleKey<L1TopoRDOCollection> m_daqRdoCollectionKey {
    this, "DAQRDOCollection", "L1TopoRDOCollection",
    "Key of the L1TopoRDOCollection object holding decoded L1Topo information from DAQ"};
  SG::ReadHandleKey<LVL1::FrontPanelCTP> m_simTopoCTPKey {
    this, "SimTopoCTPLocation", LVL1::DEFAULT_L1TopoLegacyCTPLocation,
    "Key of the simulated topo decision output for CTP"};
  SG::ReadHandleKey<LVL1::FrontPanelCTP> m_simTopoOverflowCTPKey {
    this, "SimTopoOverflowCTPLocation", LVL1::DEFAULT_L1TopoLegacyOverflowCTPLocation,
    "Key of the simulated topo overflow output for CTP"};
  SG::ReadHandleKey<CTP_RDO> m_ctpRdoKey {
    this, "CTPRDOLocation", LVL1CTP::DEFAULT_RDOOutputLocation,
    "Key of the CTP RDO object"};
  ServiceHandle<IROBDataProviderSvc> m_robDataProviderSvc {
    this, "ROBDataProvider", "ROBDataProviderSvc",
    "The ROB Data Provider service to retrieve L1Topo ROBs"};
  ToolHandle<GenericMonitoringTool> m_monTool {
    this, "MonTool", "" ,
    "Monitoring tool to create online histograms"};
  Gaudi::Property<std::vector<uint16_t>> m_roibRobModuleIds {
    this, "RoIBROBModuleIds", {0x81, 0x82, 0x91, 0x92, 0xa1, 0xa2},
    "Module IDs for RoIBuilder ROBs to retrieve for monitoring"};
  Gaudi::Property<std::vector<uint16_t>> m_daqRobModuleIds {
    this, "DAQROBModuleIds", {0x00, 0x10, 0x20},
    "Module IDs for RoIBuilder ROBs to retrieve for monitoring"};
  Gaudi::Property<bool> m_doRawMon {
    this, "doRawMon", true, "Enable L1Topo monitoring direct from ROB fragments"};
  Gaudi::Property<bool> m_doCnvMon {
    this, "doCnvMon", true, "Enable L1Topo monitoring via converters"};
  Gaudi::Property<bool> m_doSimMon {
    this, "doSimMon", true, "Enable L1Topo hardware vs simulation comparison"};
  Gaudi::Property<bool> m_doSimDaq {
    this, "doSimDaq", true, "Enable L1Topo DAQ hardware vs simulation comparison"};
  Gaudi::Property<int> m_prescaleDaqRobAccess {
    this, "PrescaleDAQROBAccess", 1, "Prescale factor for requests for DAQ ROBs: "
    "can be used to avoid overloading ROS. <1 means disabled, 1 means always, N>1 means sample only 1 in N events"};

  // ------------------------- Other private members ---------------------------
  /// L1Topo RoIBuilder ROB IDs
  std::vector<uint32_t> m_roibRobIds;
  /// L1Topo DAQ ROB IDs
  std::vector<uint32_t> m_daqRobIds;
  /// Random engine for calculating DAQ ROB access prescale
  ATHRNG::RNGWrapper m_RNGEngines;

  // ------------------------- Private methods ---------------------------------
  /// \c monitorROBs for ROIB and DAQ inputs
  StatusCode doRawMon(bool allowDAQROBAccess,
                      const EventContext& eventContext) const;
  /// Monitoring with converters: Retrieve ROIB, DAQ, and compare them
  StatusCode doCnvMon(xAOD::TrigComposite& errorFlags,
                      DecisionBits& decisionBits,
                      bool allowDAQROBAccess,
                      const EventContext& eventContext) const;
  /// Compare simulated trigger bits against the ones from ROIB
  StatusCode doSimMon(xAOD::TrigComposite& errorFlags,
                      DecisionBits& decisionBits,
                      bool allowDAQROBAccess,
                      const EventContext& eventContext) const;
  /// Compare simulated overflow bits against the ones from ROIB
  StatusCode doOverflowSimMon(DecisionBits& decisionBits,
                              const EventContext& eventContext) const;
  /// Compare simulated trigger bits against the ones from DAQ
  StatusCode doSimDaq(DecisionBits& decisionBits) const;
  /// Monitor ROB fragments: word types and payload size
  StatusCode monitorROBs(const IROBDataProviderSvc::VROBFRAG& robs, bool isROIB) const;
  /// Monitor L1Topo block info
  void monitorBlock(uint32_t sourceID, const L1TopoBlock& block, xAOD::TrigComposite& errorFlags) const;
  /// Compare two bitsets and histogram the differences. Returns a vector of indices of differing bits
  std::vector<size_t> compBitSets(const std::string& leftLabel,
                                  const std::string& rightLabel,
                                  const std::bitset<s_nTopoCTPOutputs>& left,
                                  const std::bitset<s_nTopoCTPOutputs>& right) const;
  /// Define and set all error flags to false
  void resetFlags(xAOD::TrigComposite& errorFlags) const;
};

#endif // L1TOPOONLINEMONITORING_L1TopoLegacyOnlineMonitor_h
