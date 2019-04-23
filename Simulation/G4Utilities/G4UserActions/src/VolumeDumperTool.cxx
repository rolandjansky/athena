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
        ActionToolBase<VolumeDumper>(type, name, parent)
{
        declareInterface<IG4EventActionTool>(this);
}

//---------------------------------------------------------------------------
// Create the action on request
//---------------------------------------------------------------------------
std::unique_ptr<VolumeDumper> VolumeDumperTool::makeAction()
{
        ATH_MSG_DEBUG("Making a VolumeDumper action");
        return std::make_unique<VolumeDumper>();
}

}
