#include "HIMonitoring/HIMonitoringEventShapeTool.h"
#include "HIMonitoring/HIMonitoringPhotonsTool.h"
#include "HIMonitoring/HIMonitoringZdcTool.h"
#include "HIMonitoring/HIMonitoringMuonsTool.h"
#include "HIMonitoring/HIMonitoringElectronsTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY(HIMonitoringEventShapeTool)
DECLARE_TOOL_FACTORY(HIMonitoringPhotonsTool)
DECLARE_TOOL_FACTORY(HIMonitoringZdcTool)
DECLARE_TOOL_FACTORY(HIMonitoringMuonsTool)
DECLARE_TOOL_FACTORY(HIMonitoringElectronsTool)	
	
DECLARE_FACTORY_ENTRIES(HIMonitoring) 
{
    DECLARE_ALGTOOL(HIMonitoringEventShapeTool);
	DECLARE_ALGTOOL(HIMonitoringPhotonsTool);
	DECLARE_ALGTOOL(HIMonitoringZdcTool);
	DECLARE_ALGTOOL(HIMonitoringMuonsTool);
	DECLARE_ALGTOOL(HIMonitoringElectronsTool);
}
