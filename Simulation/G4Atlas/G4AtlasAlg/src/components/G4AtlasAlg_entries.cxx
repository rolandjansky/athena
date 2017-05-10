#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../G4AtlasAlg.h"
#include "../AthenaStackingActionTool.h"
#include "../AthenaTrackingActionTool.h"

DECLARE_ALGORITHM_FACTORY( G4AtlasAlg )
DECLARE_NAMESPACE_TOOL_FACTORY( G4UA, AthenaStackingActionTool )
DECLARE_NAMESPACE_TOOL_FACTORY( G4UA, AthenaTrackingActionTool )
