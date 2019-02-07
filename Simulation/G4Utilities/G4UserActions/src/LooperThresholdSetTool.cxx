/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#include "G4UserActions/LooperThresholdSetTool.h"

namespace G4UA
{

//---------------------------------------------------------------------------
// Constructor
//---------------------------------------------------------------------------
LooperThresholdSetTool::LooperThresholdSetTool(const std::string& type, const std::string& name,
                                               const IInterface* parent) :
        UserActionToolBase<LooperThresholdSet>(type, name, parent),
        m_config()
{
        declareProperty("WarningEnergy", m_config.WarningEnergy);
        declareProperty("ImportantEnergy", m_config.ImportantEnergy);
        declareProperty("NumberOfTrials", m_config.NumberOfTrials);
}

//---------------------------------------------------------------------------
// Initialize - temporarily here for debugging
//---------------------------------------------------------------------------
StatusCode LooperThresholdSetTool::initialize()
{
        ATH_MSG_DEBUG("initialize");
        return StatusCode::SUCCESS;
}

StatusCode LooperThresholdSetTool::finalize()
{
        ATH_MSG_DEBUG("finalize");
        return StatusCode::SUCCESS;
}


//---------------------------------------------------------------------------
// Create the action on request
//---------------------------------------------------------------------------
std::unique_ptr<LooperThresholdSet>
LooperThresholdSetTool::makeAndFillAction(G4AtlasUserActions& actionList)
{
        ATH_MSG_DEBUG("Making a LooperThresholdSet action");
        auto action =  std::make_unique<LooperThresholdSet>(m_config);
        actionList.runActions.push_back( action.get() );
        return action;
}

}
