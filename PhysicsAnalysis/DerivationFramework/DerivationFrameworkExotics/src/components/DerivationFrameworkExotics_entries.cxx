#include "GaudiKernel/DeclareFactoryEntries.h"
#include "DerivationFrameworkExotics/SkimmingToolEXOT14.h"
#include "DerivationFrameworkExotics/SkimmingToolEXOT5.h"

using namespace DerivationFramework;

DECLARE_TOOL_FACTORY( SkimmingToolEXOT14 )
DECLARE_TOOL_FACTORY( SkimmingToolEXOT5 )

DECLARE_FACTORY_ENTRIES( DerivationFrameworkExotics ) {
   DECLARE_TOOL( SkimmingToolEXOT14 )
   DECLARE_TOOL( SkimmingToolEXOT5 )
}

