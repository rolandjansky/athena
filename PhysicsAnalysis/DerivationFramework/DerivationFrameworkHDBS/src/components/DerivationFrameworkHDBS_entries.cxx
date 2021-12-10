#include "GaudiKernel/DeclareFactoryEntries.h"
#include "DerivationFrameworkHDBS/SkimmingToolHDBS2.h"
#include "DerivationFrameworkHDBS/SkimmingToolHDBS3.h"

using namespace DerivationFramework;

DECLARE_TOOL_FACTORY( SkimmingToolHDBS2 )
DECLARE_TOOL_FACTORY( SkimmingToolHDBS3 )

DECLARE_FACTORY_ENTRIES( DerivationFrameworkHDBS ) {
   DECLARE_TOOL( SkimmingToolHDBS2 )
   DECLARE_TOOL( SkimmingToolHDBS3 )
}

