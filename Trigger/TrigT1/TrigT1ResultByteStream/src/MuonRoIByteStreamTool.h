/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1RESULTBYTESTREAM_MUONROIBYTESTREAMTOOL_H
#define TRIGT1RESULTBYTESTREAM_MUONROIBYTESTREAMTOOL_H

// Trigger includes
#include "TrigT1ResultByteStream/IL1TriggerByteStreamTool.h"
#include "TrigT1Interfaces/ITrigT1MuonRecRoiTool.h"
#include "TrigT1Interfaces/ITrigThresholdDecisionTool.h"
#include "xAODTrigger/MuonRoIContainer.h"

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"

// Gaudi includes
#include "Gaudi/Property.h"

/** @class MuonRoIByteStreamTool
 *  @brief Tool for converting L1Muon RoI from BS to xAOD and from xAOD to BS
 *  (IL1TriggerByteStreamTool interface)
 *
 **/
namespace LVL1 {
  class ITrigT1MuonRecRoiTool;
  class ITrigThresholdDecisionTool;
}

class MuonRoIByteStreamTool : public extends<AthAlgTool, IL1TriggerByteStreamTool> {
public:
  MuonRoIByteStreamTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~MuonRoIByteStreamTool() override = default;

  // ------------------------- IAlgTool methods --------------------------------
  virtual StatusCode initialize() override;

  // ------------------------- IL1TriggerByteStreamTool methods ----------------
  /// BS->xAOD conversion
  virtual StatusCode convertFromBS(const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vrobf,
                                   const EventContext& eventContext) const override;
  /// xAOD->BS conversion
  virtual StatusCode convertToBS(std::vector<OFFLINE_FRAGMENTS_NAMESPACE_WRITE::ROBFragment*>& vrobf,
                                 const EventContext& eventContext) override;
  /// Declare ROB IDs for conversion
  virtual const std::vector<uint32_t>& robIds() const override {return m_robIds.value();}

private:
  // ------------------------- Properties --------------------------------------
  // ROBIDs property required by the interface
  Gaudi::Property<std::vector<uint32_t>> m_robIds {
    this, "ROBIDs", {}, "List of ROB IDs required for conversion to/from xAOD RoI"};
  // It is a good idea to have also the module IDs configurable in case they change at some point
  Gaudi::Property<uint16_t> m_muCTPIModuleID {
    this, "MUCTPIModuleId", 1, "Module ID of MUCTPI ROB with RoI information"};

  // Only write key should be set to non-empty string in python configuration if the tool is in BS->xAOD mode of operation
  SG::WriteHandleKey<xAOD::MuonRoIContainer> m_roiWriteKey {
    this, "MuonRoIContainerWriteKey", "", "Write handle key to MuonRoIContainer for conversion from ByteStream"};
  // Only read key should be set to non-empty string in python configuration if the tool is in xAOD->BS mode of operation
  SG::ReadHandleKey<xAOD::MuonRoIContainer> m_roiReadKey {
    this, "MuonRoIContainerReadKey", "", "Read handle key to MuonRoIContainer for conversion to ByteStream"};

  BooleanProperty m_useRun3Config{this,"UseRun3Config",false,"use Run 3 config"};
  ToolHandle<LVL1::ITrigT1MuonRecRoiTool> m_rpcTool{this, "RPCRecRoiTool", "LVL1::TrigT1RPCRecRoiTool/LVL1__TrigT1RPCRecRoiTool", "Tool to get the eta/phi coordinates in the RPC"};
  ToolHandle<LVL1::ITrigT1MuonRecRoiTool> m_tgcTool{this, "TGCRecRoiTool", "LVL1::TrigT1TGCRecRoiTool/LVL1__TrigT1TGCRecRoiTool", "Tool to get the eta/phi coordinates in the TGC"};
  ToolHandle<LVL1::ITrigThresholdDecisionTool> m_thresholdTool{this, "TrigThresholdDecisionTool", "LVL1::TrigThresholdDecisionTool/LVL1__TrigThresholdDecisionTool", "Tool to get pass/fail of each trigger threshold"};

};

#endif // TRIGT1RESULTBYTESTREAM_MUONROIBYTESTREAMTOOL_H
