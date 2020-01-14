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
#include "GaudiKernel/Property.h"

/** @class ExampleL1TriggerByteStreamTool
 *  @brief Example implementation of a tool for L1 RoI conversion from BS to xAOD and from xAOD to BS
 *  (IL1TriggerByteStreamTool interface)
 *
 *  This example decodes Muon RoIs from MUCTPI raw data, filling the results with dummy values. Real implementations
 *  should have very similar structure and should implement the same functionality and properties. In particular,
 *  their BS->xAOD convert method should also record a new xAOD collection in the event store and link it to the
 *  L1TriggerResult object, as presented here.
 **/
class ExampleL1TriggerByteStreamTool : public extends<AthAlgTool, IL1TriggerByteStreamTool> {
public:
  ExampleL1TriggerByteStreamTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~ExampleL1TriggerByteStreamTool() override = default;

  // ------------------------- IAlgTool methods --------------------------------
  virtual StatusCode initialize() override;

  // ------------------------- IL1TriggerByteStreamTool methods ----------------
  /// BS->xAOD conversion
  virtual StatusCode convert(const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vrobf,
                             xAOD::TrigComposite& l1TriggerResult,
                             const EventContext& eventContext) const override;
  /// xAOD->BS conversion
  virtual StatusCode convert(const xAOD::TrigComposite& l1TriggerResult,
                             std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vrobf,
                             const EventContext& eventContext) const override;
  /// Declare ROB IDs for conversion
  virtual const std::vector<uint32_t> robIds() const override {return m_robIds.value();}
  /// Declare name of the link from L1TriggerResult to the xAOD RoI
  virtual const std::string linkName() const override {return m_linkName.value();}

private:
  // ------------------------- Properties --------------------------------------
  // The following two are required by the interface
  Gaudi::Property<std::vector<uint32_t>> m_robIds {
    this, "ROBIDs", {}, "List of ROB IDs required for conversion to/from xAOD RoI"};
  Gaudi::Property<std::string> m_linkName {
    this, "LinkName", "UNDEFINED_LINK_NAME", "Name of the link from L1TriggerResult to the xAOD RoI"};

  /// Write key should be reset to empty string in python configuration if the tool is in xAOD->BS mode of operation
  SG::WriteHandleKey<xAOD::MuonRoIContainer> m_roiWriteKey {
    this, "MuonRoIContainerWriteKey", "LVL1MuonRoIs", "Write handle key to MuonRoIContainer for conversion from ByteStream"};

  // It is a good idea to have the module IDs configurable in case they change at some point
  /// MUCTPI Module ID to decode
  Gaudi::Property<uint16_t> m_muCTPIModuleID {
    this, "MUCTPIModuleId", 1, "Module ID of MUCTPI ROB with RoI information"
  };
};

#endif // TRIGT1RESULTBYTESTREAM_EXAMPLEL1TRIGGERBYTESTREAMTOOL_H
