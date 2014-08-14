
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TRT_SeededSpacePointFinderTool/TRT_SeededSpacePointFinder_ATL.h"
#include "TRT_SeededSpacePointFinderTool/SimpleTRT_SeededSpacePointFinder_ATL.h"

using namespace InDet;

DECLARE_TOOL_FACTORY( TRT_SeededSpacePointFinder_ATL )
DECLARE_TOOL_FACTORY( SimpleTRT_SeededSpacePointFinder_ATL )

DECLARE_FACTORY_ENTRIES( TRT_SeededSpacePointFinderTool)
{
	DECLARE_TOOL( TRT_SeededSpacePointFinder_ATL )
	DECLARE_TOOL( SimpleTRT_SeededSpacePointFinder_ATL )
}





