/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef L1DECODER_JROISUNPACKINGTOOL_H
#define L1DECODER_JROISUNPACKINGTOOL_H 1
#include <string>

#include "TrigConfInterfaces/ILVL1ConfigSvc.h"
#include "TrigConfL1Data/ThresholdConfig.h"
#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigT1Interfaces/RecJetRoI.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "AthenaMonitoring/GenericMonitoringTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "RoIsUnpackingToolBase.h"


class JRoIsUnpackingTool : public RoIsUnpackingToolBase { 
public: 

  JRoIsUnpackingTool( const std::string& type,
                      const std::string& name, 
                      const IInterface* parent );

  StatusCode unpack(const EventContext& ctx,
                    const ROIB::RoIBResult& roib,
                    const HLT::IDSet& activeChains) const override;
  
  virtual StatusCode initialize() override;
  virtual StatusCode updateConfiguration() override;
  virtual StatusCode finalize() override { return StatusCode::SUCCESS; }

private: 
  SG::WriteHandleKey<TrigRoiDescriptorCollection> m_trigRoIsKey{
    this, "OutputTrigRoIs", "JETRoIs", "Name of the RoIs object produced by the unpacker"};

  SG::WriteHandleKey<TrigRoiDescriptorCollection> m_trigFSRoIsKey{
    this, "OutputFSTrigRoIs", "FSJETRoIs", "Name of the RoIs object containing the single FS RoI tagged with all jet chains produced by the unpacker"};


  SG::WriteHandleKey< DataVector<LVL1::RecJetRoI> > m_recRoIsKey{
    this, "OutputRecRoIs", "RecJETRoIs", "Name of the RoIs object produced by the unpacker"};

  SG::WriteHandleKey< TrigCompositeUtils::DecisionContainer > m_fsDecisions{
    this, "FSDecisions", "FSJetDecisions", "Decisions for the single FS RoI"
  };

  Gaudi::Property<float>            m_roIWidth{
    this, "RoIWidth", 0.4, "Size of RoI in eta/ phi"};
  ///@}
  
  ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc;
  std::vector<TrigConf::TriggerThreshold*> m_jetThresholds;

}; 


#endif //> !L1DECODER_JROISUNPACKINGTOOL_H
