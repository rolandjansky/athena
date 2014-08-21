#include "GaudiKernel/DeclareFactoryEntries.h"
#include "ISF_FatrasToolsID/SimHitCreatorID.h"
#include "ISF_FatrasToolsID/HitCreatorSilicon.h"
#include "ISF_FatrasToolsID/HitCreatorTRT.h"

DECLARE_NAMESPACE_TOOL_FACTORY( iFatras, SimHitCreatorID )
DECLARE_NAMESPACE_TOOL_FACTORY( iFatras, HitCreatorSilicon )
DECLARE_NAMESPACE_TOOL_FACTORY( iFatras, HitCreatorTRT )

DECLARE_FACTORY_ENTRIES( ISF_FatrasToolsID )
{
  DECLARE_NAMESPACE_TOOL( iFatras, SimHitCreatorID )
  DECLARE_NAMESPACE_TOOL( iFatras, HitCreatorSilicon )
  DECLARE_NAMESPACE_TOOL( iFatras, HitCreatorTRT )
}

