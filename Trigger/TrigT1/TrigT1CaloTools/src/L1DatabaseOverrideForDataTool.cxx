/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include "L1DatabaseOverrideForDataTool.h"

#include "TrigT1CaloCalibConditions/L1CaloPprChanDefaultsContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloPprChanCalibContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloDisabledTowersContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloPpmDeadChannelsContainer.h"

namespace LVL1 {

L1DatabaseOverrideForDataTool::L1DatabaseOverrideForDataTool(
                          const std::string& t,
			  const std::string& n,
			  const IInterface*  p )
  :  AthAlgTool(t,n,p)
{
  declareInterface<IL1DatabaseOverrideTool>(this);
}

L1DatabaseOverrideForDataTool::~L1DatabaseOverrideForDataTool()
{
}

// this tool only modifies ChanDefaults, the rest is a straight-copy of the input
StatusCode L1DatabaseOverrideForDataTool::modifyChanCalib(L1CaloPprChanCalibContainer&)
{
  return StatusCode::SUCCESS;
}
StatusCode L1DatabaseOverrideForDataTool::modifyDisabledTowers(L1CaloDisabledTowersContainer&)
{
  return StatusCode::SUCCESS;
}
StatusCode L1DatabaseOverrideForDataTool::modifyDeadChannels(L1CaloPpmDeadChannelsContainer&)
{
  return StatusCode::SUCCESS;
}

StatusCode L1DatabaseOverrideForDataTool::modifyChanDefaults(L1CaloPprChanDefaultsContainer& c)
{
  for(auto& chan : c) {
    // swap bcidDecision1 and bcidDecision3
    auto bcidDecision1 = chan.bcidDecision1(), bcidDecision3 = chan.bcidDecision3();
    chan.setBcidDecision1(bcidDecision3);
    chan.setBcidDecision3(bcidDecision1);

    // swap satOverride1 and satOverride3
    auto satOverride1 = chan.satOverride1(), satOverride3 = chan.satOverride3();
    chan.setSatOverride1(satOverride3);
    chan.setSatOverride3(satOverride1);
  }
  return StatusCode::SUCCESS;
}

} // end of namespace LVL1
