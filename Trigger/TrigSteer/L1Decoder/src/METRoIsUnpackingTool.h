/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef L1DECODER_METROISUNPACKINGTOOL_H
#define L1DECODER_METROISUNPACKINGTOOL_H 1

#include "TrigConfInterfaces/ILVL1ConfigSvc.h"
#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigT1Interfaces/JEPRoIDecoder.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "RoIsUnpackingToolBase.h"
#include <string>


class METRoIsUnpackingTool : public RoIsUnpackingToolBase { 
public: 

  METRoIsUnpackingTool( const std::string& type,
			const std::string& name, 
			const IInterface* parent );
  
  virtual StatusCode updateConfiguration() override;
  
  virtual StatusCode initialize() override;

  StatusCode unpack(const EventContext& ctx,
                    const ROIB::RoIBResult& roib,
                    const HLT::IDSet& activeChains) const override;

private: 
  SG::WriteHandleKey<TrigRoiDescriptorCollection> m_trigRoIsKey{
    this, "OutputTrigRoI", "METRoI", "Name of the RoI object produced by the unpacker"};

  ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc;
  LVL1::JEPRoIDecoder m_jepDecoder;
  HLT::IDSet m_allMETChains;
  std::vector<const TrigConf::TriggerThreshold*> m_thresholds;
}; 


#endif //> !L1DECODER_JROISUNPACKINGTOOL_H
