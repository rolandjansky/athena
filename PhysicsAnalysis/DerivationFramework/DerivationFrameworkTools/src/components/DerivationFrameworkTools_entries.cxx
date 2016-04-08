#include "GaudiKernel/DeclareFactoryEntries.h"
#include "DerivationFrameworkTools/xAODStringSkimmingTool.h"
#include "DerivationFrameworkTools/NTUPStringSkimmingTool.h"
#include "DerivationFrameworkTools/InvariantMassTool.h"
#include "DerivationFrameworkTools/DeltaRTool.h"
#include "DerivationFrameworkTools/AsgSelectionToolWrapper.h"
#include "DerivationFrameworkTools/FilterCombinationAND.h"
#include "DerivationFrameworkTools/FilterCombinationOR.h"
#include "DerivationFrameworkTools/TriggerSkimmingTool.h"
#include "DerivationFrameworkTools/PrescaleTool.h"
#include "DerivationFrameworkTools/GenericObjectThinning.h"

using namespace DerivationFramework;

DECLARE_TOOL_FACTORY( xAODStringSkimmingTool )
DECLARE_TOOL_FACTORY( NTUPStringSkimmingTool )
DECLARE_TOOL_FACTORY( InvariantMassTool )
DECLARE_TOOL_FACTORY( DeltaRTool )
DECLARE_TOOL_FACTORY( AsgSelectionToolWrapper )
DECLARE_TOOL_FACTORY( FilterCombinationAND )
DECLARE_TOOL_FACTORY( FilterCombinationOR )
DECLARE_TOOL_FACTORY( TriggerSkimmingTool )
DECLARE_TOOL_FACTORY( PrescaleTool )
DECLARE_TOOL_FACTORY( GenericObjectThinning )

DECLARE_FACTORY_ENTRIES( DerivationFrameworkTools ) {
   DECLARE_TOOL( xAODStringSkimmingTool )
   DECLARE_TOOL( NTUPStringSkimmingTool )
   DECLARE_TOOL( InvariantMassTool )
   DECLARE_TOOL( DeltaRTool )
   DECLARE_TOOL( AsgSelectionToolWrapper )
   DECLARE_TOOL( FilterCombinationAND )
   DECLARE_TOOL( FilterCombinationOR )
   DECLARE_TOOL( TriggerSkimmingTool )
   DECLARE_TOOL( PrescaleTool )
   DECLARE_TOOL( GenericObjectThinning )
}

