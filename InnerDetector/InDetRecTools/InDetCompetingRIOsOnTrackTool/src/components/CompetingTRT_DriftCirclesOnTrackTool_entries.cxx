#include "GaudiKernel/DeclareFactoryEntries.h"
#include "InDetCompetingRIOsOnTrackTool/CompetingTRT_DriftCirclesOnTrackTool.h"
#include "InDetCompetingRIOsOnTrackTool/CompetingPixelClustersOnTrackTool.h"
#include "InDetCompetingRIOsOnTrackTool/CompetingSCT_ClustersOnTrackTool.h"

//using namespace InDet;

DECLARE_NAMESPACE_TOOL_FACTORY( InDet, CompetingTRT_DriftCirclesOnTrackTool )
DECLARE_NAMESPACE_TOOL_FACTORY( InDet, CompetingPixelClustersOnTrackTool )
DECLARE_NAMESPACE_TOOL_FACTORY( InDet, CompetingSCT_ClustersOnTrackTool )

DECLARE_FACTORY_ENTRIES(InDetCompetingRIOsOnTrackTool)
{ // in this package name these methods are registered 
  DECLARE_NAMESPACE_TOOL(InDet, CompetingTRT_DriftCirclesOnTrackTool)
  DECLARE_NAMESPACE_TOOL(InDet, CompetingPixelClustersOnTrackTool)
  DECLARE_NAMESPACE_TOOL(InDet, CompetingSCT_ClustersOnTrackTool)
}
