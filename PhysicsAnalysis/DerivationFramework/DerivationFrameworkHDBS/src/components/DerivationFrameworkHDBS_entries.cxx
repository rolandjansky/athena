#include "GaudiKernel/DeclareFactoryEntries.h"
<<<<<<< PhysicsAnalysis/DerivationFramework/DerivationFrameworkHDBS/src/components/DerivationFrameworkHDBS_entries.cxx
#include "DerivationFrameworkHDBS/SkimmingToolHDBS2.h"
#include "DerivationFrameworkHDBS/SkimmingToolHDBS3.h"
#include "DerivationFrameworkHDBS/HDBSa0Finder.h"

using namespace DerivationFramework;

DECLARE_TOOL_FACTORY( SkimmingToolHDBS2 )
DECLARE_TOOL_FACTORY( SkimmingToolHDBS3 )
DECLARE_TOOL_FACTORY( HDBSa0Finder )

DECLARE_FACTORY_ENTRIES( DerivationFrameworkHDBS ) {
   DECLARE_TOOL( SkimmingToolHDBS2 )
   DECLARE_TOOL( SkimmingToolHDBS3 )
   DECLARE_TOOL( HDBSa0Finder )
=======
#include "DerivationFrameworkHDBS/SkimmingToolHDBS2.h"
#include "DerivationFrameworkHDBS/SkimmingToolHDBS3.h"

using namespace DerivationFramework;

DECLARE_TOOL_FACTORY( SkimmingToolHDBS2 )
DECLARE_TOOL_FACTORY( SkimmingToolHDBS3 )

DECLARE_FACTORY_ENTRIES( DerivationFrameworkHDBS ) {
   DECLARE_TOOL( SkimmingToolHDBS2 )
   DECLARE_TOOL( SkimmingToolHDBS3 )
>>>>>>> PhysicsAnalysis/DerivationFramework/DerivationFrameworkHDBS/src/components/DerivationFrameworkHDBS_entries.cxx
}
