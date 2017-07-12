/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef L1DECODER_MUROISUNPACKINGTOOL_H
#define L1DECODER_MUROISUNPACKINGTOOL_H 1


#include <string>
#include "TrigConfInterfaces/ILVL1ConfigSvc.h"
#include "TrigConfL1Data/ThresholdConfig.h"
#include "TrigConfL1Data/TriggerThreshold.h"

#include "TrigT1Interfaces/RecMuonRoI.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"


#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "TrigT1Interfaces/RecMuonRoiSvc.h"
#include "AthenaMonitoring/GenericMonitoringTool.h"

#include "IRoIsUnpackingTool.h"


class MURoIsUnpackingTool
  : virtual public ::IRoIsUnpackingTool,
    public ::AthAlgTool
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  MURoIsUnpackingTool( const std::string& type,
		       const std::string& name, 
		       const IInterface* parent );

  /// Destructor: 
  virtual ~MURoIsUnpackingTool(); 
  
  // Athena algtool's Hooks
  StatusCode initialize() override;
  StatusCode updateConfiguration() override;
  StatusCode finalize() override;
  StatusCode unpack(const EventContext& ctx,
		    const ROIB::RoIBResult& roib,
		    const HLT::IDSet& activeChains) const override;
private: 

  /// Default constructor: 
  MURoIsUnpackingTool();
  
  std::vector<TrigConf::TriggerThreshold*> m_muonThresholds;
  SG::WriteHandleKey< TrigRoiDescriptorCollection > m_trigRoIsKey;
  SG::WriteHandleKey< DataVector<LVL1::RecMuonRoI> > m_recRoIsKey;
  ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc;
  ServiceHandle<LVL1::RecMuonRoiSvc> m_recRpcRoISvc;
  ServiceHandle<LVL1::RecMuonRoiSvc> m_recTgcRoISvc;
  float m_roIWidth;
  ToolHandle<GenericMonitoringTool> m_monTool;
}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 


#endif //> !L1DECODER_MUROISUNPACKINGTOOL_H
