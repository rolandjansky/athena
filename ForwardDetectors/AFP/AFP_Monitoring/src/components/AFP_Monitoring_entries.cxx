#include "GaudiKernel/DeclareFactoryEntries.h"
#include "AFP_Monitoring/AFPHitsMonitorTool.h"
#include "AFP_Monitoring/AFPTechnicalMonitorTool.h"

DECLARE_TOOL_FACTORY(AFPHitsMonitorTool)
DECLARE_TOOL_FACTORY(AFPTechnicalMonitorTool)

DECLARE_FACTORY_ENTRIES(AFP_Monitoring) {
  DECLARE_ALGTOOL(AFPHitsMonitorTool)
    DECLARE_ALGTOOL(AFPTechnicalMonitorTool)
}
