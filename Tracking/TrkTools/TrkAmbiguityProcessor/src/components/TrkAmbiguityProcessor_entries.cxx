#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../SimpleAmbiguityProcessorTool.h"
#include "../TrackSelectionProcessorTool.h"
#include "../TrackScoringTool.h"
#include "../DenseEnvironmentsAmbiguityProcessorTool.h"
#include "../SimpleFitterTool.h"

using namespace Trk;
DECLARE_TOOL_FACTORY(DenseEnvironmentsAmbiguityProcessorTool)
DECLARE_TOOL_FACTORY(SimpleAmbiguityProcessorTool)
DECLARE_TOOL_FACTORY(TrackSelectionProcessorTool)
DECLARE_TOOL_FACTORY(TrackScoringTool)
DECLARE_TOOL_FACTORY(SimpleFitterTool)

DECLARE_FACTORY_ENTRIES( TrkAmbiguityProcessor )
{
  DECLARE_TOOL(DenseEnvironmentsAmbiguityProcessorTool)
	DECLARE_TOOL(SimpleAmbiguityProcessorTool)
	DECLARE_TOOL(TrackSelectionProcessorTool)
	DECLARE_TOOL(TrackScoringTool)
  DECLARE_TOOL(SimpleFitterTool)
}




