/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef L1DECODER_JROISUNPACKINGTOOL_H
#define L1DECODER_JROISUNPACKINGTOOL_H 1
#include <string>

#include "TrigConfInterfaces/ILVL1ConfigSvc.h"
#include "TrigConfL1Data/ThresholdConfig.h"
#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigT1Interfaces/RecJetRoI.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "RoIsUnpackingToolBase.h"


class JRoIsUnpackingTool : public RoIsUnpackingToolBase {
public:

  JRoIsUnpackingTool( const std::string& type,
                      const std::string& name,
                      const IInterface* parent );

  using RoIsUnpackingToolBase::unpack;
  StatusCode unpack(const EventContext& ctx,
                    const ROIB::RoIBResult& roib,
                    const HLT::IDSet& activeChains) const override;

  virtual StatusCode initialize() override;
  virtual StatusCode updateConfiguration() override;
  virtual StatusCode finalize() override { return StatusCode::SUCCESS; }
  virtual StatusCode start() override;
private:
  SG::WriteHandleKey<TrigRoiDescriptorCollection> m_trigRoIsKey{
    this, "OutputTrigRoIs", "HLT_JETRoIs",
    "Name of the RoIs object produced by the unpacker"};


  SG::WriteHandleKey< DataVector<LVL1::RecJetRoI> > m_recRoIsKey{
    this, "OutputRecRoIs", "HLT_RecJETRoIs",
    "Name of the RoIs object produced by the unpacker"};

  Gaudi::Property<float> m_roIWidth{
    this, "RoIWidth", 0.4, "Size of RoI in eta/ phi"};


  Gaudi::Property<std::string> m_fsRoIKey{
    this, "FSRoIKey", "HLT_FSRoI",
    "The key of FS RoI made earlier by the L1Decoder" };

  ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc;
  std::vector<TrigConf::TriggerThreshold*> m_jetThresholds;

};


#endif //> !L1DECODER_JROISUNPACKINGTOOL_H
