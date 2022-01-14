/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef HLTSEEDING_MUROISUNPACKINGTOOL_H
#define HLTSEEDING_MUROISUNPACKINGTOOL_H

#include "RoIsUnpackingToolBase.h"

#include "TrigT1Interfaces/RecMuonRoI.h"
#include "TrigT1Interfaces/ITrigT1MuonRecRoiTool.h"
#include "xAODTrigger/MuonRoIContainer.h"

#include "StoreGate/ReadDecorHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

#include "GaudiKernel/ToolHandle.h"

#include <string>

class MURoIsUnpackingTool : public RoIsUnpackingToolBase {
 public:
  /// Constructor with parameters:
  MURoIsUnpackingTool(const std::string& type,
                      const std::string& name,
                      const IInterface* parent);

  virtual StatusCode initialize() override;
  virtual StatusCode start() override;

  using RoIsUnpackingToolBase::unpack;
  virtual StatusCode unpack(const EventContext& ctx,
                            const ROIB::RoIBResult& roib,
                            const HLT::IDSet& activeChains) const override;
private:
  /// @name Data dependencies
  /// @{
  SG::WriteHandleKey< DataVector<LVL1::RecMuonRoI> > m_recRoIsKey{
    this, "OutputRecRoIs", "HLT_RecMURoIs", "Name of the RoIs object produced by the unpacker"};
  /// @}

  /// @name Properties
  /// @{
  Gaudi::Property<float> m_roIWidth{
    this, "RoIWidth", 0.1, "Size of RoI in eta/ phi"};
  /// @}

  /// @name Tools
  /// @{
  ToolHandle<LVL1::ITrigT1MuonRecRoiTool> m_recRpcRoITool{
    this, "RecRpcRoiTool", "LVL1::TrigT1RPCRecRoiTool/TrigT1RPCRecRoiTool"};
  ToolHandle<LVL1::ITrigT1MuonRecRoiTool> m_recTgcRoITool{
    this, "TgcRpcRoiTool", "LVL1::TrigT1TGCRecRoiTool/TrigT1TGCRecRoiTool"};
  /// @}
};

#endif //> !HLTSEEDING_MUROISUNPACKINGTOOL_H
