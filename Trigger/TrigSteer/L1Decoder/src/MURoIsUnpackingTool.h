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
private: 

  ///@{ @name Properties
  SG::WriteHandleKey< TrigRoiDescriptorCollection > m_trigRoIsKey{
    this, "OutputTrigRoIs", "MURoIs", "Name of the RoIs object produced by the unpacker"};

  SG::WriteHandleKey< DataVector<LVL1::RecMuonRoI> > m_recRoIsKey{
    this, "OutputRecRoIs", "RecMURoIs", "Name of the RoIs object produced by the unpacker"};

  Gaudi::Property<float> m_roIWidth{"RoIWidth", 0.1, "Size of RoI in eta/ phi"};
  ///@}

  ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc;
  ServiceHandle<LVL1::RecMuonRoiSvc> m_recRpcRoISvc;
  ServiceHandle<LVL1::RecMuonRoiSvc> m_recTgcRoISvc;

  std::vector<TrigConf::TriggerThreshold*> m_muonThresholds;
}; 


#endif //> !L1DECODER_MUROISUNPACKINGTOOL_H
