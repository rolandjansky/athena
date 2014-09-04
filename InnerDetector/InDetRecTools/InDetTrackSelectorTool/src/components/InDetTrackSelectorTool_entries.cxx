#include "GaudiKernel/DeclareFactoryEntries.h"
#include "InDetTrackSelectorTool/InDetTrackSelectorTool.h"
#include "InDetTrackSelectorTool/InDetIsoTrackSelectorTool.h"
#include "InDetTrackSelectorTool/InDetCosmicTrackSelectorTool.h"
#include "InDetTrackSelectorTool/InDetDetailedTrackSelectorTool.h"
#include "InDetTrackSelectorTool/InDetConversionTrackSelectorTool.h"
#include "InDetTrackSelectorTool/InDetTrtDriftCircleCutTool.h"

using namespace InDet;

DECLARE_TOOL_FACTORY( InDetTrackSelectorTool )
DECLARE_TOOL_FACTORY( InDetIsoTrackSelectorTool )
DECLARE_TOOL_FACTORY( InDetCosmicTrackSelectorTool )
DECLARE_TOOL_FACTORY( InDetDetailedTrackSelectorTool )
DECLARE_TOOL_FACTORY( InDetConversionTrackSelectorTool )
DECLARE_TOOL_FACTORY( InDetTrtDriftCircleCutTool )

DECLARE_FACTORY_ENTRIES( InDetTrackSelectorTool )  
{ 
  DECLARE_TOOL( InDetTrackSelectorTool );
  DECLARE_TOOL( InDetIsoTrackSelectorTool );
  DECLARE_TOOL( InDetCosmicTrackSelectorTool );
  DECLARE_TOOL( InDetDetailedTrackSelectorTool );
  DECLARE_TOOL( InDetConversionTrackSelectorTool );
  DECLARE_TOOL( InDetTrtDriftCircleCutTool );
}
