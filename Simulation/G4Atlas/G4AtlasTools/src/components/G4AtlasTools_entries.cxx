#include "GaudiKernel/DeclareFactoryEntries.h"

#include "G4AtlasTools/PhysicsListToolBase.h"
#include "G4AtlasTools/UserActionBase.h"
#include "G4AtlasTools/AddPhysicsDecayTool.h"
#include "../SensitiveDetectorMasterTool.h"
#include "../FastSimulationMasterTool.h"

DECLARE_TOOL_FACTORY( PhysicsListToolBase )
DECLARE_TOOL_FACTORY( UserActionBase )
DECLARE_TOOL_FACTORY( SensitiveDetectorMasterTool )
DECLARE_TOOL_FACTORY( FastSimulationMasterTool )
DECLARE_TOOL_FACTORY( AddPhysicsDecayTool )

DECLARE_FACTORY_ENTRIES( G4AtlasTools ) {
  DECLARE_TOOL( PhysicsListToolBase )
  DECLARE_TOOL( UserActionBase )
  DECLARE_TOOL( SensitiveDetectorMasterTool )
  DECLARE_TOOL( FastSimulationMasterTool )
  DECLARE_TOOL( AddPhysicsDecayTool )
}
