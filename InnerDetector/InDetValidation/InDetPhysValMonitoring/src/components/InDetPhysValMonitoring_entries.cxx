#include "InDetPhysValMonitoring/InDetPhysValMonitoringTool.h"
#include "InDetPhysValMonitoring/InDetPhysValLargeD0Tool.h"
#include "../InDetPhysValTruthDecoratorTool.h"
#include "../InDetPhysHitDecoratorTool.h"
#include "../ParameterErrDecoratorTool.h"
#include "../TruthClassDecoratorTool.h"
#include "InDetPhysValMonitoring/InDetPhysValDecoratorAlg.h"
#include "../TrackTruthSelectionTool.h"
#include "../dRMatchingTool.h"
#include "../TrackSelectionTool.h"
#include "InDetPhysValMonitoring/HistogramDefinitionSvc.h"
#include "InDetPhysValMonitoring/AlgTestHistoDefSvc.h"
#include "InDetPhysValMonitoring/ToolTestMonitoringPlots.h"
#include "../AthTruthSelectionTool.h"

#include "../DummyTrackSlimmingTool.h"

DECLARE_COMPONENT( InDetPhysValDecoratorAlg )
DECLARE_COMPONENT( AlgTestHistoDefSvc )
DECLARE_COMPONENT( HistogramDefinitionSvc )
DECLARE_COMPONENT( InDetPhysValMonitoringTool )
DECLARE_COMPONENT( InDetPhysValLargeD0Tool )
DECLARE_COMPONENT( ToolTestMonitoringPlots )
DECLARE_COMPONENT( InDetPhysValTruthDecoratorTool )
DECLARE_COMPONENT( InDetPhysHitDecoratorTool )
DECLARE_COMPONENT( ParameterErrDecoratorTool )
DECLARE_COMPONENT( TruthClassDecoratorTool )
DECLARE_COMPONENT( TrackTruthSelectionTool )
DECLARE_COMPONENT( dRMatchingTool )
DECLARE_COMPONENT( TrackSelectionTool )
DECLARE_COMPONENT( DummyTrackSlimmingTool )
DECLARE_COMPONENT( AthTruthSelectionTool )

