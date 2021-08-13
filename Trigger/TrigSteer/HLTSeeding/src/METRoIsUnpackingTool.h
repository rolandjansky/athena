/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef HLTSEEDING_METROISUNPACKINGTOOL_H
#define HLTSEEDING_METROISUNPACKINGTOOL_H 1

#include "RoIsUnpackingToolBase.h"
#include "TrigT1Interfaces/JEPRoIDecoder.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"

#include <string>

class METRoIsUnpackingTool : public RoIsUnpackingToolBase {
public:
  METRoIsUnpackingTool( const std::string& type,
			const std::string& name,
			const IInterface* parent );

  virtual StatusCode initialize() override;
  virtual StatusCode start() override;

  using RoIsUnpackingToolBase::unpack;
  StatusCode unpack(const EventContext& ctx,
                    const ROIB::RoIBResult& roib,
                    const HLT::IDSet& activeChains) const override;

private:
  Gaudi::Property<std::string> m_fsRoIKey{ this, "FSRoIKey", "HLT_FSRoI", "The key of FS RoI made earlier by the HLTSeeding" };

  LVL1::JEPRoIDecoder m_jepDecoder;
  HLT::IDSet m_allMETChains;
};

#endif //> !HLTSEEDING_JROISUNPACKINGTOOL_H
