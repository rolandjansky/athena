// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "../SUSYPhysValMonitoring/SUSYPhysValMonitoring.h"

/// ---------------------------- release 17 legacy code
#include "../SUSYPhysValMonitoring/rel17_SUSYPlot.h"
#include "../SUSYPhysValMonitoring/rel17_SUSYSusyRec.h"
///

// #include "GaudiKernel/DeclareFactoryEntries.h"


DECLARE_NAMESPACE_TOOL_FACTORY( SUSYPhysVal, SUSYPhysValMonitoring )

/// ---------------------------- release 17 legacy code
DECLARE_ALGORITHM_FACTORY( SUSYPlot )
// DECLARE_TOOL_FACTORY( SUSYGlobalTool )
DECLARE_TOOL_FACTORY( SUSYSusyRec )
///


DECLARE_FACTORY_ENTRIES( SUSYPhysValMonitoring ) {

  DECLARE_NAMESPACE_TOOL( SUSYPhysVal, SUSYPhysValMonitoring )

  /// ---------------------------- release 17 legacy code
  DECLARE_ALGORITHM( SUSYPlot );
//   DECLARE_TOOL( SUSYGlobalTool );
  DECLARE_TOOL( SUSYSusyRec );
  ///

}

