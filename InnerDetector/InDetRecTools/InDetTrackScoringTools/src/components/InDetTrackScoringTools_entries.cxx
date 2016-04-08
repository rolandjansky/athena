
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "InDetTrackScoringTools/InDetAmbiScoringTool.h"
#include "InDetTrackScoringTools/InDetCosmicScoringTool.h"
#include "InDetTrackScoringTools/InDetTrtTrackScoringTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY( InDet, InDetAmbiScoringTool )
DECLARE_NAMESPACE_TOOL_FACTORY( InDet, InDetCosmicScoringTool )
DECLARE_NAMESPACE_TOOL_FACTORY( InDet, InDetTrtTrackScoringTool )

DECLARE_FACTORY_ENTRIES( InDetTrackScoringTools )
{
	DECLARE_NAMESPACE_TOOL( InDet, InDetAmbiScoringTool )
	DECLARE_NAMESPACE_TOOL( InDet, InDetCosmicScoringTool )
	DECLARE_NAMESPACE_TOOL( InDet, InDetTrtTrackScoringTool )
}




