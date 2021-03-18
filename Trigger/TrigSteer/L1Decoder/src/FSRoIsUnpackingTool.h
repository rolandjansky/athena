/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef L1DECODER_FSROISUNPACKINGTOOL_H
#define L1DECODER_FSROISUNPACKINGTOOL_H 1

#include "TrigConfInterfaces/ILVL1ConfigSvc.h"
#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigT1Interfaces/JEPRoIDecoder.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "RoIsUnpackingToolBase.h"
#include <string>


class FSRoIsUnpackingTool : public RoIsUnpackingToolBase {
public:

  FSRoIsUnpackingTool( const std::string& type,
			const std::string& name,
			const IInterface* parent );

  virtual StatusCode updateConfiguration() override;

  virtual StatusCode initialize() override;

  using RoIsUnpackingToolBase::unpack;
  StatusCode unpack(const EventContext& ctx,
                    const ROIB::RoIBResult& roib,
                    const HLT::IDSet& activeChains) const override;
  virtual StatusCode start() override;

private:
  SG::WriteHandleKey<TrigRoiDescriptorCollection> m_fsRoIKey{
    this, "OutputTrigRoIs", "HLT_FSRoI", "The key of FS RoI" };

  ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc;
  HLT::IDSet m_allFSChains;

};


#endif //> !L1DECODER_JROISUNPACKINGTOOL_H
