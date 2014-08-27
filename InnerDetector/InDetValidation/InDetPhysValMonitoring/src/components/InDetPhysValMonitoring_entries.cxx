#include "GaudiKernel/DeclareFactoryEntries.h"
#include "InDetPhysValMonitoring/InDetPhysValMonitoringTool.h"
#include "../InDetPhysValTruthDecoratorTool.h"
#include "../InDetPhysHitDecoratorTool.h"
#include "../ParameterErrDecoratorTool.h"
#include "../TruthClassDecoratorTool.h"

DECLARE_TOOL_FACTORY(InDetPhysValMonitoringTool)
DECLARE_TOOL_FACTORY(InDetPhysValTruthDecoratorTool)
DECLARE_TOOL_FACTORY(InDetPhysHitDecoratorTool)
DECLARE_TOOL_FACTORY(ParameterErrDecoratorTool)
DECLARE_TOOL_FACTORY(TruthClassDecoratorTool)
	
DECLARE_FACTORY_ENTRIES(InDetPhysValMonitoring){
	DECLARE_TOOL(InDetPhysValMonitoringTool)
	DECLARE_TOOL(InDetPhysValTruthDecoratorTool)
	DECLARE_TOOL(InDetPhysHitDecoratorTool)
	DECLARE_TOOL(ParameterErrDecoratorTool)
	DECLARE_TOOL(TruthClassDecoratorTool)

}
