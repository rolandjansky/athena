/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef HLTSEEDING_FSROISUNPACKINGTOOL_H
#define HLTSEEDING_FSROISUNPACKINGTOOL_H 1

#include "RoIsUnpackingToolBase.h"

class FSRoIsUnpackingTool : public RoIsUnpackingToolBase {
public:

  FSRoIsUnpackingTool( const std::string& type,
			const std::string& name,
			const IInterface* parent );

  virtual StatusCode initialize() override;
  virtual StatusCode start() override;

  using RoIsUnpackingToolBase::unpack;
  StatusCode unpack(const EventContext& ctx,
                    const ROIB::RoIBResult& roib,
                    const HLT::IDSet& activeChains) const override;

private:
  HLT::IDSet m_allFSChains;
};

#endif
