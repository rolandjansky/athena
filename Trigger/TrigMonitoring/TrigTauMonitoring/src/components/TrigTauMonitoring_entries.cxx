
#include "../HLTTauMonTool.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY(HLTTauMonTool )

DECLARE_FACTORY_ENTRIES(TrigTauMonitoring) {
  DECLARE_ALGTOOL( HLTTauMonTool )
}

