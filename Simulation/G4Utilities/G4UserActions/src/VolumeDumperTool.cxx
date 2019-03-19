/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#include "G4UserActions/VolumeDumperTool.h"

namespace G4UA
{

//---------------------------------------------------------------------------
// Constructor
//---------------------------------------------------------------------------
VolumeDumperTool::VolumeDumperTool(const std::string& type, const std::string& name,
                                   const IInterface* parent) :
        UserActionToolBase<VolumeDumper>(type, name, parent)
{
}

//---------------------------------------------------------------------------
// Create the action on request
//---------------------------------------------------------------------------
std::unique_ptr<VolumeDumper>
VolumeDumperTool::makeAndFillAction(G4AtlasUserActions& actionList)
{
        ATH_MSG_DEBUG("Constructing a VolumeDumper action");
        auto action = std::make_unique<VolumeDumper>();
        actionList.eventActions.push_back( action.get() );
        return action;
}

}
