/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//author: @asonay
//email: anil.sonay@cern.ch

#ifndef L1TOPOPHASE1BYTESTREAMTOOL_H 
#define L1TOPOPHASE1BYTESTREAMTOOL_H

// Trigger includes
#include "TrigT1ResultByteStream/IL1TriggerByteStreamTool.h"

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"
// Gaudi includes
#include "Gaudi/Property.h"

//xAOD
#include "xAODTrigL1Calo/L1TopoRawDataContainer.h"
#include "xAODTrigL1Calo/L1TopoRawDataAuxContainer.h"

#include "CxxUtils/span.h"

using ROBF = OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;
using WROBF = OFFLINE_FRAGMENTS_NAMESPACE_WRITE::ROBFragment;

class L1TopoPhase1ByteStreamTool : public extends<AthAlgTool, IL1TriggerByteStreamTool> {

public:
  L1TopoPhase1ByteStreamTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~L1TopoPhase1ByteStreamTool() override = default;
  // ------------------------- IAlgTool methods ---------------------------------
  virtual StatusCode initialize() override;
  // ------------------------- IL1TriggerByteStreamTool methods -----------------
  /// BS->xAOD conversion
  virtual StatusCode convertFromBS(const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vrobf, const EventContext& eventContext)const override;
  /// xAOD->BS conversion
  virtual StatusCode convertToBS(std::vector<OFFLINE_FRAGMENTS_NAMESPACE_WRITE::ROBFragment*>& vrobf, const EventContext& eventContext) override;
  /// Declare ROB IDs for conversion
  virtual const std::vector<uint32_t>& robIds() const override {
    return m_robIds.value();
  }
private:
  // ------------------------- Private Methods ---------------------------------
  StatusCode convert(const ROBF* rob, const std::unique_ptr<xAOD::L1TopoRawDataContainer> &container) const;
  
  // ------------------------- Properties --------------------------------------
  // ROBIDs property required by the interface
    Gaudi::Property<std::vector<uint32_t>> m_robIds {this, "ROBIDs", {}, "List of ROB IDs required for conversion to/from xAOD RoI"};
	
  // Write handle keys for the L1TopoRAWDataContainer for BS->xAOD mode of operation
  SG::WriteHandleKey< xAOD::L1TopoRawDataContainer > m_topoRawWriteKey   {this,"L1TopoPhase1RAWDataWriteContainer"  ,"L1_Phase1L1TopoRAWData","Write Phase1 L1Topo RAW data"};

  // Read handle keys for the L1Calo EDMs for xAOD->BS mode of operation
  SG::ReadHandleKey< xAOD::L1TopoRawDataContainer > m_topoRawReadKey   {this,"L1TopoPhase1RAWDataReadContainer"  ,"L1_Phase1L1TopoRAWData","Read Phase1 L1Topo RAW data"};

  //------------------------- Private Vars ---------------------------------------
  
};
#endif // L1TOPOPHASE1BYTESTREAMTOOL_H
