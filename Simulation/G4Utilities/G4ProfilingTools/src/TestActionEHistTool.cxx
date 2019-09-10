/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TestActionEHistTool.h"

namespace G4UA
{

  TestActionEHistTool::TestActionEHistTool(const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent)
    : UserActionToolBase<TestActionEHist>(type, name, parent)
  {
    declareProperty("ROOTFileName",m_config.name);
    declareProperty("CaloDepth",m_config.dCALO);
    declareProperty("BeamPipeDepth",m_config.dBeam);
    declareProperty("InDetDepth",m_config.dIDET);
    declareProperty("MuonDepth",m_config.dMUON);
    declareProperty("MaxHists",m_config.maxhists);
    declareProperty("DetailDepth",m_config.dDetail);
  }

  std::unique_ptr<TestActionEHist>
  TestActionEHistTool::makeAndFillAction(G4AtlasUserActions& actionList)
  {
    ATH_MSG_DEBUG("Constructing a TestActionEHist");
    auto action = std::make_unique<TestActionEHist>(m_config);
    actionList.runActions.push_back( action.get() );
    actionList.trackingActions.push_back( action.get() );
    actionList.steppingActions.push_back( action.get() );
    return action;
  }

} // namespace G4UA
