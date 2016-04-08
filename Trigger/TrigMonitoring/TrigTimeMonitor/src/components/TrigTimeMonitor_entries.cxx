#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrigTimeMonitor/TrigTimeTreeTool.h"
#include "TrigTimeMonitor/TrigTimeHistTool.h"

DECLARE_TOOL_FACTORY( TrigTimeTreeTool )
DECLARE_TOOL_FACTORY( TrigTimeHistTool )

DECLARE_FACTORY_ENTRIES(TrigTimeMonitor) {
    DECLARE_ALGTOOL( TrigTimeTreeTool )
    DECLARE_ALGTOOL( TrigTimeHistTool )
}
