/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#ifndef G4USERACTIONS_G4UA_VOLUMEDUMPERTOOL_H
#define G4USERACTIONS_G4UA_VOLUMEDUMPERTOOL_H

// System includes
#include <string>

// Infrastructure includes
#include "G4AtlasInterfaces/IG4EventActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"

// Local includes
#include "G4UserActions/VolumeDumper.h"

namespace G4UA
{

/// @class VolumeDumperTool
/// @brief Tool which manages the volume dumper action.
///
///
/// @author Miha Muskinja
///
class VolumeDumperTool : public ActionToolBase<VolumeDumper>,
                         public IG4EventActionTool
{

public:

/// Standard constructor
VolumeDumperTool(const std::string& type,
                 const std::string& name,
                 const IInterface* parent);

/// retrieves the Event action
virtual G4UserEventAction* getEventAction() override final
{
        return static_cast<G4UserEventAction*>( getAction() );
}

protected:

/// Create action for this thread
virtual std::unique_ptr<VolumeDumper> makeAction() override final;

}; // class VolumeDumperTool

} // namespace G4UA

#endif
