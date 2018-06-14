#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../SimHitCreatorMS.h"

DECLARE_NAMESPACE_TOOL_FACTORY( iFatras, SimHitCreatorMS )

DECLARE_FACTORY_ENTRIES( ISF_FatrasToolsMS )
{
  DECLARE_NAMESPACE_TOOL( iFatras, SimHitCreatorMS )
}

