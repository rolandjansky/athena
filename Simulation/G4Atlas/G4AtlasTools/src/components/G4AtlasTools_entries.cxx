#include "GaudiKernel/DeclareFactoryEntries.h"

#include "G4AtlasTools/PhysicsListToolBase.h"
#include "G4AtlasTools/SensitiveDetectorBase.h"
#include "G4AtlasTools/FastSimulationBase.h"

DECLARE_TOOL_FACTORY( PhysicsListToolBase )
DECLARE_TOOL_FACTORY( SensitiveDetectorBase )
DECLARE_TOOL_FACTORY( FastSimulationBase )

DECLARE_FACTORY_ENTRIES( G4AtlasTools ) {
  DECLARE_TOOL( PhysicsListToolBase )
  DECLARE_TOOL( SensitiveDetectorBase )
  DECLARE_TOOL( FastSimulationBase )
}
