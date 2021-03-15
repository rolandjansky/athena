/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef L1DECODER_MUROISUNPACKINGTOOL_H
#define L1DECODER_MUROISUNPACKINGTOOL_H


#include <string>
#include "TrigConfInterfaces/ILVL1ConfigSvc.h"
#include "TrigConfL1Data/ThresholdConfig.h"
#include "TrigConfL1Data/TriggerThreshold.h"

#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "TrigT1Interfaces/ITrigT1MuonRecRoiTool.h"

#include "RoIsUnpackingToolBase.h"


class MURoIsUnpackingTool : public RoIsUnpackingToolBase
{ 
 public: 
  /// Constructor with parameters: 
  MURoIsUnpackingTool( const std::string& type,
                       const std::string& name, 
                       const IInterface* parent );

  virtual StatusCode initialize() override;
  virtual StatusCode updateConfiguration() override;
  virtual StatusCode unpack(const EventContext& ctx,
                            const ROIB::RoIBResult& roib,
                            const HLT::IDSet& activeChains) const override;
  virtual StatusCode unpack(const EventContext& ctx,
                            const xAOD::TrigComposite& l1TriggerResult,
                            const HLT::IDSet& activeChains) const override;
  virtual StatusCode start() override;
private: 

  ///@{ @name Properties
  SG::WriteHandleKey< TrigRoiDescriptorCollection > m_trigRoIsKey{
    this, "OutputTrigRoIs", "HLT_MURoIs",
    "Name of the RoIs object produced by the unpacker"};

  SG::WriteHandleKey< DataVector<LVL1::RecMuonRoI> > m_recRoIsKey{
    this, "OutputRecRoIs", "HLT_RecMURoIs",
    "Name of the RoIs object produced by the unpacker"};

  Gaudi::Property<std::string> m_muRoILinkName {
    this, "MuRoILinkName", "LVL1MuonRoIs",
    "Name of the link to read from L1TriggerResult for muon RoI container"};

  Gaudi::Property<float> m_roIWidth{
    this, "RoIWidth", 0.1, "Size of RoI in eta/ phi"};
  ///@}

  ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc;
  ToolHandle<LVL1::ITrigT1MuonRecRoiTool> m_recRpcRoITool{this, "RecRpcRoiTool", "LVL1::TrigT1RPCRecRoiTool/TrigT1RPCRecRoiTool"};
  ToolHandle<LVL1::ITrigT1MuonRecRoiTool> m_recTgcRoITool{this, "TgcRpcRoiTool", "LVL1::TrigT1TGCRecRoiTool/TrigT1TGCRecRoiTool"};

  std::vector<TrigConf::TriggerThreshold*> m_muonThresholds;
}; 


#endif //> !L1DECODER_MUROISUNPACKINGTOOL_H
