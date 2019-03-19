/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#ifndef G4USERACTIONS_G4UA_VOLUMEDUMPERTOOL_H
#define G4USERACTIONS_G4UA_VOLUMEDUMPERTOOL_H

// System includes
#include <string>

// Infrastructure includes
#include "G4AtlasTools/UserActionToolBase.h"

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
class VolumeDumperTool : public UserActionToolBase<VolumeDumper>
{

public:

/// Standard constructor
VolumeDumperTool(const std::string& type,
                 const std::string& name,
                 const IInterface* parent);

protected:

/// creates the action instances
virtual std::unique_ptr<VolumeDumper>
makeAndFillAction(G4AtlasUserActions&) override final;

}; // class VolumeDumperTool

} // namespace G4UA

#endif
