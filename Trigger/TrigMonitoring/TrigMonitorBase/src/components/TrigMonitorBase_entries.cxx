#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../TrigHLTBaseTreeTool.h"
#include "../TrigGenericMonitoringTool.h"

DECLARE_TOOL_FACTORY( TrigHLTBaseTreeTool )
DECLARE_TOOL_FACTORY( TrigGenericMonitoringTool )

DECLARE_FACTORY_ENTRIES(TrigMonitorBase) {
    DECLARE_ALGTOOL( TrigHLTBaseTreeTool )
    DECLARE_ALGTOOL( TrigGenericMonitoringTool )  
}

