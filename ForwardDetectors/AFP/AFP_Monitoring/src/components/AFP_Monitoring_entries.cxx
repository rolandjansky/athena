#include "GaudiKernel/DeclareFactoryEntries.h"
#include "AFP_Monitoring/AFPHitsMonitorTool.h"
#include "AFP_Monitoring/AFPTechnicalMonitorTool.h"
#include "AFP_Monitoring/AFPSiLayerMonitor.h"
#include "AFP_Monitoring/AFPSiStationMonitor.h"

DECLARE_TOOL_FACTORY(AFPHitsMonitorTool)
DECLARE_TOOL_FACTORY(AFPTechnicalMonitorTool)
DECLARE_TOOL_FACTORY(AFPSiLayerMonitor)
DECLARE_TOOL_FACTORY(AFPSiStationMonitor)

DECLARE_FACTORY_ENTRIES(AFP_Monitoring) {
  DECLARE_ALGTOOL(AFPHitsMonitorTool)
  DECLARE_ALGTOOL(AFPSiLayerMonitor)
  DECLARE_ALGTOOL(AFPSiStationMonitor)
  DECLARE_ALGTOOL(AFPTechnicalMonitorTool)
}
