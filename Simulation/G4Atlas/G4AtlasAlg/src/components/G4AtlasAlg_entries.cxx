#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../G4AtlasAlg.h"
#include "../AthenaStackingAction.h"
#include "../AthenaTrackingAction.h"
#include "../AthenaStackingActionTool.h"
#include "../AthenaTrackingActionTool.h"

DECLARE_FACTORY_ENTRIES(G4AtlasAlg) {
  DECLARE_ALGORITHM( G4AtlasAlg )
  DECLARE_TOOL( AthenaStackingAction )
  DECLARE_TOOL( AthenaTrackingAction )
  DECLARE_TOOL( G4UA::AthenaStackingActionTool )
  DECLARE_TOOL( G4UA::AthenaTrackingActionTool )
}

DECLARE_ALGORITHM_FACTORY( G4AtlasAlg )
DECLARE_TOOL_FACTORY( AthenaStackingAction )
DECLARE_TOOL_FACTORY( AthenaTrackingAction )
DECLARE_TOOL_FACTORY( G4UA::AthenaStackingActionTool )
DECLARE_TOOL_FACTORY( G4UA::AthenaTrackingActionTool )
