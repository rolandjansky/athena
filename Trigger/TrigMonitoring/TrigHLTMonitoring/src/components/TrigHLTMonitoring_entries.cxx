#include "TrigHLTMonitoring/IHLTMonTool.h"
#include "TrigHLTMonitoring/HLTMonTool.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY(HLTMonTool )

DECLARE_FACTORY_ENTRIES(TrigHLTMonitoring) {
DECLARE_ALGTOOL( HLTMonTool )
}

