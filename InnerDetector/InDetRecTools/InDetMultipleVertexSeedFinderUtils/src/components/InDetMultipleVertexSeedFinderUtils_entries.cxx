#include "GaudiKernel/DeclareFactoryEntries.h"
#include "InDetMultipleVertexSeedFinderUtils/InDetTrackZ0SortingTool.h"
#include "InDetMultipleVertexSeedFinderUtils/InDetTrackClusterCleaningTool.h"

using namespace InDet;

DECLARE_TOOL_FACTORY( InDetTrackZ0SortingTool )
DECLARE_TOOL_FACTORY( InDetTrackClusterCleaningTool )

DECLARE_FACTORY_ENTRIES( InDetMultipleVertexSeedFinderUtils )  
{ 
  DECLARE_TOOL( InDetTrackZ0SortingTool );
  DECLARE_TOOL( InDetTrackClusterCleaningTool );
}
