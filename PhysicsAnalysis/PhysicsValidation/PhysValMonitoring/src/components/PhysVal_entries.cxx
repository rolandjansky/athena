// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "../PhysValExample.h"

DECLARE_NAMESPACE_TOOL_FACTORY( PhysVal, PhysValExample )

DECLARE_FACTORY_ENTRIES( PhysValMonitoring ) {

  DECLARE_NAMESPACE_TOOL( PhysVal, PhysValExample )

}
