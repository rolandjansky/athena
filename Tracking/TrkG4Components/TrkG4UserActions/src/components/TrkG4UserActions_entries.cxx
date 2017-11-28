#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrkG4UserActions/MaterialStepRecorderTool.h"
#include "TrkG4UserActions/EnergyLossRecorderTool.h"
#include "TrkG4UserActions/GeantFollowerTool.h"
#include "TrkG4UserActions/GeantFollowerHelper.h"
#include "TrkG4UserActions/GeantFollowerMSTool.h"
#include "TrkG4UserActions/GeantFollowerMSHelper.h"

DECLARE_TOOL_FACTORY( G4UA::MaterialStepRecorderTool )
DECLARE_TOOL_FACTORY( G4UA::EnergyLossRecorderTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk , GeantFollowerHelper )
DECLARE_TOOL_FACTORY( G4UA::GeantFollowerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk , GeantFollowerMSHelper )
DECLARE_TOOL_FACTORY( G4UA::GeantFollowerMSTool )

DECLARE_FACTORY_ENTRIES( TrkG4UserActions ) {
    DECLARE_TOOL( G4UA::MaterialStepRecorderTool )
    DECLARE_TOOL( G4UA::EnergyLossRecorderTool )
    DECLARE_TOOL( G4UA::GeantFollowerTool )
    DECLARE_NAMESPACE_TOOL( Trk , GeantFollowerHelper )
    DECLARE_TOOL( G4UA::GeantFollowerMSTool )
    DECLARE_NAMESPACE_TOOL( Trk , GeantFollowerMSHelper )
}
