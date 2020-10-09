/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1RESULTBYTESTREAM_EXAMPLEL1TRIGGERBYTESTREAMTOOL_H
#define TRIGT1RESULTBYTESTREAM_EXAMPLEL1TRIGGERBYTESTREAMTOOL_H

// Trigger includes
#include "TrigT1ResultByteStream/IL1TriggerByteStreamTool.h"
#include "xAODTrigger/MuonRoIContainer.h"

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"

// Gaudi includes
#include "Gaudi/Property.h"

/** @class ExampleL1TriggerByteStreamTool
 *  @brief Example implementation of a tool for L1 RoI conversion from BS to xAOD and from xAOD to BS
 *  (IL1TriggerByteStreamTool interface)
 *
 *  This example decodes Muon RoIs from MUCTPI raw data, filling the results with dummy values. Real implementations
 *  should have very similar structure and should implement the same functionality and properties. In particular,
 *  the convertFromBS method should record a new xAOD collection in the event store using a WriteHandle, and the
 *  convertToBS method should take the xAOD collection from the event store using a ReadHandle.
 **/
class ExampleL1TriggerByteStreamTool : public extends<AthAlgTool, IL1TriggerByteStreamTool> {
public:
  ExampleL1TriggerByteStreamTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~ExampleL1TriggerByteStreamTool() override = default;

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

};

#endif // TRIGT1RESULTBYTESTREAM_EXAMPLEL1TRIGGERBYTESTREAMTOOL_H
