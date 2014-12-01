#include "GaudiKernel/DeclareFactoryEntries.h"
#include "InDetAmbiTrackSelectionTool/InDetAmbiTrackSelectionTool.h"
#include "InDetAmbiTrackSelectionTool/InDetDenseEnvAmbiTrackSelectionTool.h"


DECLARE_NAMESPACE_TOOL_FACTORY( InDet, InDetAmbiTrackSelectionTool )
DECLARE_NAMESPACE_TOOL_FACTORY( InDet, InDetDenseEnvAmbiTrackSelectionTool )


DECLARE_FACTORY_ENTRIES( InDetAmbiTrackSelectionTool )
{
	DECLARE_NAMESPACE_TOOL( InDet, InDetAmbiTrackSelectionTool )
	DECLARE_NAMESPACE_TOOL( InDet, InDetDenseEnvAmbiTrackSelectionTool )
}

