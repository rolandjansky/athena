/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef L1DECODER_TAUROISUNPACKINGTOOL_H
#define L1DECODER_TAUROISUNPACKINGTOOL_H 1

// STL includes
#include <string>

#include "TrigConfInterfaces/ILVL1ConfigSvc.h"
#include "TrigConfL1Data/ThresholdConfig.h"
#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigT1Interfaces/RecEmTauRoI.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "AthenaMonitoring/GenericMonitoringTool.h"

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

// L1Decoder includes
#include "RoIsUnpackingToolBase.h"


class TAURoIsUnpackingTool : public RoIsUnpackingToolBase { 
public: 

  TAURoIsUnpackingTool(const std::string& type,
                      const std::string& name, 
                      const IInterface* parent);

  StatusCode unpack(const EventContext& ctx,
                    const ROIB::RoIBResult& roib,
                    const HLT::IDSet& activeChains) const override;
  
  virtual StatusCode initialize() override;
  virtual StatusCode updateConfiguration() override;
  virtual StatusCode finalize() override;
  
private: 

  ///@{ @name Properties
  SG::WriteHandleKey<TrigRoiDescriptorCollection> m_trigRoIsKey{
    this, "OutputTrigRoIs", "TAURoIs", "Name of the RoIs object produced by the unpacker"};

  SG::WriteHandleKey< DataVector<LVL1::RecEmTauRoI> > m_recRoIsKey{
    this, "OutputRecRoIs", "RecTAURoIs", "Name of the RoIs object produced by the unpacker"};

  Gaudi::Property<float>            m_roIWidth{this, "RoIWidth", 0.1, "Size of RoI in eta/ phi"};
  ///@}
  
  ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc;
  std::vector<TrigConf::TriggerThreshold*> m_emThresholds;

}; 

#endif //> !L1DECODER_TAUROISUNPACKINGTOOL_H
