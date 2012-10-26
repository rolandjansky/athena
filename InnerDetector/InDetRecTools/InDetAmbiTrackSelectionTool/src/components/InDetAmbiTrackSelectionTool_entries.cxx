#include "GaudiKernel/DeclareFactoryEntries.h"
#include "InDetAmbiTrackSelectionTool/InDetAmbiTrackSelectionTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY( InDet, InDetAmbiTrackSelectionTool )

DECLARE_FACTORY_ENTRIES( InDetAmbiTrackSelectionTool )
{
	DECLARE_NAMESPACE_TOOL( InDet, InDetAmbiTrackSelectionTool )
}

