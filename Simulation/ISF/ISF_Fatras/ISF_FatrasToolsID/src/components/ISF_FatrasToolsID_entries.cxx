#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../SimHitCreatorID.h"
#include "../HitCreatorSilicon.h"
#include "../HitCreatorTRT.h"

DECLARE_NAMESPACE_TOOL_FACTORY( iFatras, SimHitCreatorID )
DECLARE_NAMESPACE_TOOL_FACTORY( iFatras, HitCreatorSilicon )
DECLARE_NAMESPACE_TOOL_FACTORY( iFatras, HitCreatorTRT )

DECLARE_FACTORY_ENTRIES( ISF_FatrasToolsID )
{
  DECLARE_NAMESPACE_TOOL( iFatras, SimHitCreatorID )
  DECLARE_NAMESPACE_TOOL( iFatras, HitCreatorSilicon )
  DECLARE_NAMESPACE_TOOL( iFatras, HitCreatorTRT )
}

