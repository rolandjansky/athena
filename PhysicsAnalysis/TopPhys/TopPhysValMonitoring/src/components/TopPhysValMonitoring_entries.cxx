// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "TopPhysValMonitoring/TopPhysValMonitoring.h"

DECLARE_NAMESPACE_TOOL_FACTORY( TopPhysVal, TopPhysValMonitoring )

DECLARE_FACTORY_ENTRIES( TopPhysValMonitoring ) {

  DECLARE_NAMESPACE_TOOL( TopPhysVal, TopPhysValMonitoring )

}
