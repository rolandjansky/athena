#include "GaudiKernel/DeclareFactoryEntries.h"
#include "InDetPhysValMonitoring/InDetPhysValMonitoringTool.h"
#include "InDetPhysValMonitoring/InDetItkMonitoringTool.h"
#include "../InDetPhysValTruthDecoratorTool.h"
#include "../InDetPhysHitDecoratorTool.h"
#include "../ParameterErrDecoratorTool.h"
#include "../TruthClassDecoratorTool.h"
#include "InDetPhysValMonitoring/InDetPhysValDecoratorAlg.h"
#include "../TrackTruthSelectionTool.h"
#include "InDetPhysValMonitoring/HistogramDefinitionSvc.h"
#include "InDetPhysValMonitoring/AlgTestHistoDefSvc.h"
#include "InDetPhysValMonitoring/ToolTestMonitoringPlots.h"

#include "../DummyTrackSlimmingTool.h"

DECLARE_ALGORITHM_FACTORY(InDetPhysValDecoratorAlg)
DECLARE_ALGORITHM_FACTORY(AlgTestHistoDefSvc)

DECLARE_SERVICE_FACTORY(HistogramDefinitionSvc)

DECLARE_TOOL_FACTORY(InDetPhysValMonitoringTool)
DECLARE_TOOL_FACTORY(InDetItkMonitoringTool)
DECLARE_TOOL_FACTORY(ToolTestMonitoringPlots)

DECLARE_TOOL_FACTORY(InDetPhysValTruthDecoratorTool)
DECLARE_TOOL_FACTORY(InDetPhysHitDecoratorTool)
DECLARE_TOOL_FACTORY(ParameterErrDecoratorTool)
DECLARE_TOOL_FACTORY(TruthClassDecoratorTool)
DECLARE_TOOL_FACTORY(TrackTruthSelectionTool)

DECLARE_TOOL_FACTORY( DummyTrackSlimmingTool )

DECLARE_FACTORY_ENTRIES(InDetPhysValMonitoring){
  DECLARE_SERVICE(HistogramDefinitionSvc)
	DECLARE_TOOL(InDetPhysValMonitoringTool)
	DECLARE_TOOL(InDetItkMonitoringTool)
  DECLARE_TOOL(ToolTestMonitoringPlots)

	
	DECLARE_TOOL(InDetPhysValTruthDecoratorTool)
	DECLARE_TOOL(InDetPhysHitDecoratorTool)
	DECLARE_TOOL(ParameterErrDecoratorTool)
	DECLARE_TOOL(TruthClassDecoratorTool)
	DECLARE_TOOL(TrackTruthSelectionTool);

        DECLARE_TOOL( DummyTrackSlimmingTool )

	DECLARE_ALGORITHM( InDetPhysValDecoratorAlg )
	DECLARE_ALGORITHM( AlgTestHistoDefSvc )

}
