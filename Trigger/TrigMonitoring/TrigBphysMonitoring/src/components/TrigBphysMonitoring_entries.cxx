#include "GaudiKernel/DeclareFactoryEntries.h"

#include "TrigBphysMonitoring/HLTXAODBphysMonTool.h"


DECLARE_TOOL_FACTORY( HLTXAODBphysMonTool )

DECLARE_FACTORY_ENTRIES(TrigBphysMonitoring) {
  DECLARE_ALGTOOL( HLTXAODBphysMonTool )
}
