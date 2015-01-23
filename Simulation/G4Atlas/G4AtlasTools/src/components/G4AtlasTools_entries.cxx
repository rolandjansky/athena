#include "GaudiKernel/DeclareFactoryEntries.h"

#include "G4AtlasTools/PhysicsListToolBase.h"
#include "G4AtlasTools/SensitiveDetectorBase.h"
#include "G4AtlasTools/FastSimulationBase.h"
#include "G4AtlasTools/UserActionBase.h"


DECLARE_TOOL_FACTORY( PhysicsListToolBase )
DECLARE_TOOL_FACTORY( SensitiveDetectorBase )
DECLARE_TOOL_FACTORY( FastSimulationBase )
DECLARE_TOOL_FACTORY( UserActionBase )

DECLARE_FACTORY_ENTRIES( G4AtlasTools ) {
  DECLARE_TOOL( PhysicsListToolBase )
  DECLARE_TOOL( SensitiveDetectorBase )
  DECLARE_TOOL( FastSimulationBase )
  DECLARE_TOOL( UserActionBase )
}
