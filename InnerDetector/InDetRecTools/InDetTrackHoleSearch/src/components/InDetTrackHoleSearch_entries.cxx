#include "GaudiKernel/DeclareFactoryEntries.h"
#include "InDetTrackHoleSearch/InDetTrackHoleSearchTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY( InDet, InDetTrackHoleSearchTool )

DECLARE_FACTORY_ENTRIES( InDetTrackHoleSearch )
{
	DECLARE_NAMESPACE_TOOL( InDet, InDetTrackHoleSearchTool )
}

