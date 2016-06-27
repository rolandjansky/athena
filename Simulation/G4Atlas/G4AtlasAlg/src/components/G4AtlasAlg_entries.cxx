#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../G4AtlasAlg.h"
#include "../AthenaStackingAction.h"
#include "../AthenaTrackingAction.h"
#include "../AthenaStackingActionTool.h"
#include "../AthenaTrackingActionTool.h"

DECLARE_ALGORITHM_FACTORY( G4AtlasAlg )
DECLARE_TOOL_FACTORY( AthenaStackingAction )
DECLARE_TOOL_FACTORY( AthenaTrackingAction )
DECLARE_NAMESPACE_TOOL_FACTORY( G4UA, AthenaStackingActionTool )
DECLARE_NAMESPACE_TOOL_FACTORY( G4UA, AthenaTrackingActionTool )
