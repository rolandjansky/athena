#include "GaudiKernel/DeclareFactoryEntries.h"
#include "DerivationFrameworkTools/xAODStringSkimmingTool.h"
#include "DerivationFrameworkTools/NTUPStringSkimmingTool.h"
#include "DerivationFrameworkTools/InvariantMassTool.h"
#include "DerivationFrameworkTools/DeltaRTool.h"
#include "DerivationFrameworkTools/NTUPThinningTool.h"
#include "DerivationFrameworkTools/AsgSelectionToolWrapper.h"

using namespace DerivationFramework;

DECLARE_TOOL_FACTORY( xAODStringSkimmingTool )
DECLARE_TOOL_FACTORY( NTUPStringSkimmingTool )
DECLARE_TOOL_FACTORY( InvariantMassTool )
DECLARE_TOOL_FACTORY( DeltaRTool )
DECLARE_TOOL_FACTORY( NTUPThinningTool )
DECLARE_TOOL_FACTORY( AsgSelectionToolWrapper )

DECLARE_FACTORY_ENTRIES( DerivationFrameworkTools ) {
   DECLARE_TOOL( xAODStringSkimmingTool )
   DECLARE_TOOL( NTUPStringSkimmingTool )
   DECLARE_TOOL( InvariantMassTool )
   DECLARE_TOOL( DeltaRTool )
   DECLARE_TOOL( NTUPThinningTool ) 
   DECLARE_TOOL( AsgSelectionToolWrapper )
}

