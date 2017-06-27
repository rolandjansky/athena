/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef L1DECODER_EMROISUNPACKINGTOOL_H
#define L1DECODER_EMROISUNPACKINGTOOL_H 1

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
#include "./IRoIsUnpackingTool.h"


class EMRoIsUnpackingTool : virtual public AthAlgTool, virtual public IRoIsUnpackingTool { 


 public: 
  EMRoIsUnpackingTool( const std::string& type,
		       const std::string& name, 
		       const IInterface* parent );

  virtual ~EMRoIsUnpackingTool(); 

  StatusCode unpack(const EventContext& ctx,
		    const ROIB::RoIBResult& roib,
		    const HLT::IDSet& activeChains) const override;
  
  // Athena algtool's Hooks
  StatusCode  initialize() override;
  StatusCode  updateConfiguration() override;
  StatusCode  finalize() override;
  
 private: 
  EMRoIsUnpackingTool();
  std::vector<TrigConf::TriggerThreshold*> m_emThresholds;
  SG::WriteHandleKey< TrigRoiDescriptorCollection > m_trigRoIsKey;
  SG::WriteHandleKey< DataVector<LVL1::RecEmTauRoI> > m_recRoIsKey;  
  ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc;
  float m_roIWidth;
  ToolHandle<GenericMonitoringTool> m_monTool;
}; 

#endif //> !L1DECODER_EMROISUNPACKINGTOOL_H
