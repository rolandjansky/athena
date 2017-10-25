#include "MissingETPerformance/MissingETPerformance.h"
#include "MissingETPerformance/MissingETData.h"
#include "MissingETPerformance/MissingETMuonData.h"
#include "MissingETPerformance/MissingETEventSelector.h"
#include "MissingETPerformance/ZeeSequencer.h"
#include "MissingETPerformance/SplitElectronContainer.h"
#include "MissingETPerformance/ResolutionTool.h"
#include "MissingETPerformance/BasicPlotsTool.h"
#include "MissingETPerformance/MissingETCompositionTool.h"
#include "MissingETPerformance/LinearityTool.h"
#include "MissingETPerformance/EtaRingsTool.h"
#include "MissingETPerformance/JetVariablesTool.h"
#include "MissingETPerformance/TopoClusterVariablesTool.h"
#include "MissingETPerformance/TrigMissingETTool.h"
#include "MissingETPerformance/TrigVsOfflineMissingETTool.h"
#include "MissingETPerformance/FakeMissingETTool.h"
#include "MissingETPerformance/ZMuMuTool.h"
#include "MissingETPerformance/ZeeTool.h"
#include "MissingETPerformance/MuonTool.h"
#include "MissingETPerformance/ContainerComparatorTool.h"
#include "MissingETPerformance/PileUpTool.h"
#include "MissingETPerformance/MissingETScaleTool.h"


DECLARE_ALGORITHM_FACTORY( MissingETPerformance )
DECLARE_ALGORITHM_FACTORY( ZeeSequencer )
DECLARE_ALGORITHM_FACTORY( SplitElectronContainer )

DECLARE_TOOL_FACTORY( MissingETData )
DECLARE_TOOL_FACTORY( MissingETMuonData )
DECLARE_TOOL_FACTORY( MissingETEventSelector )
DECLARE_TOOL_FACTORY( ResolutionTool )
DECLARE_TOOL_FACTORY( BasicPlotsTool )
DECLARE_TOOL_FACTORY( MissingETCompositionTool )
DECLARE_TOOL_FACTORY( LinearityTool )
DECLARE_TOOL_FACTORY( EtaRingsTool )
DECLARE_TOOL_FACTORY( JetVariablesTool )
DECLARE_TOOL_FACTORY( TopoClusterVariablesTool )
DECLARE_TOOL_FACTORY( TrigMissingETTool )
DECLARE_TOOL_FACTORY( TrigVsOfflineMissingETTool )
DECLARE_TOOL_FACTORY( FakeMissingETTool )
DECLARE_TOOL_FACTORY( ZMuMuTool )
DECLARE_TOOL_FACTORY( ZeeTool )
DECLARE_TOOL_FACTORY( MuonTool )
DECLARE_TOOL_FACTORY( ContainerComparatorTool )
DECLARE_TOOL_FACTORY( PileUpTool )
DECLARE_TOOL_FACTORY( MissingETScaleTool )

