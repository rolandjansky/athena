#include "SCT_TrigSpacePointTool.h"
#include "SiTrigSpacePointFormation/SiTrigSpacePointFinder.h"

#include "GaudiKernel/DeclareFactoryEntries.h"


DECLARE_NAMESPACE_TOOL_FACTORY( InDet, SCT_TrigSpacePointTool )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( InDet, SiTrigSpacePointFinder )

DECLARE_FACTORY_ENTRIES(SiTrigSpacePointFormation)
{
  DECLARE_NAMESPACE_ALGORITHM( InDet, SiTrigSpacePointFinder )
  DECLARE_NAMESPACE_TOOL( InDet, SCT_TrigSpacePointTool )

}


