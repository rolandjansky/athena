#include "GaudiKernel/DeclareFactoryEntries.h"
#include "DerivationFrameworkHDBS/SkimmingToolHDBS2.h"
#include "DerivationFrameworkHDBS/HDBSa0Finder.h"

using namespace DerivationFramework;

DECLARE_TOOL_FACTORY( SkimmingToolHDBS2 )
DECLARE_TOOL_FACTORY( HDBSa0Finder )

DECLARE_FACTORY_ENTRIES( DerivationFrameworkHDBS ) {
   DECLARE_TOOL( SkimmingToolHDBS2 )
   DECLARE_TOOL( HDBSa0Finder )
}


