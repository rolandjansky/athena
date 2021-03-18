/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef L1DECODER_EMROISUNPACKINGTOOL_H
#define L1DECODER_EMROISUNPACKINGTOOL_H 1

// STL includes
#include <string>
#include <functional>
#include <vector>

#include "TrigConfInterfaces/ILVL1ConfigSvc.h"
#include "TrigT1Interfaces/RecEmTauRoI.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

// FrameWork includes
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/WriteHandleKey.h"

// L1Decoder includes
#include "RoIsUnpackingToolBase.h"


class EMRoIsUnpackingTool : public RoIsUnpackingToolBase { 
public: 

  EMRoIsUnpackingTool(const std::string& type,
                      const std::string& name, 
                      const IInterface* parent);

  using RoIsUnpackingToolBase::unpack;
  StatusCode unpack(const EventContext& ctx,
                    const ROIB::RoIBResult& roib,
                    const HLT::IDSet& activeChains) const override;
  
  virtual StatusCode initialize() override;
  virtual StatusCode updateConfiguration() override;
  virtual StatusCode finalize() override;
  virtual StatusCode start() override;
  
private: 

  ///@{ @name Properties
  SG::WriteHandleKey<TrigRoiDescriptorCollection> m_trigRoIsKey{
    this, "OutputTrigRoIs", "HLT_EMRoIs",
    "Name of the RoIs object produced by the unpacker"};

  SG::WriteHandleKey< DataVector<LVL1::RecEmTauRoI> > m_recRoIsKey{
    this, "OutputRecRoIs", "HLT_RecEMRoIs",
    "Name of the RoIs object produced by the unpacker"};

  Gaudi::Property<float> m_roIWidth{
    this, "RoIWidth", 0.1, "Size of RoI in eta/ phi"};
  ///@}
  
  ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc;
  std::vector<TrigConf::TriggerThreshold*> m_emThresholds;
}; 

#endif //> !L1DECODER_EMROISUNPACKINGTOOL_H
