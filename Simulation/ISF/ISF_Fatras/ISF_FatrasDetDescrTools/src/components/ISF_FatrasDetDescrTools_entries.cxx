#include "GaudiKernel/DeclareFactoryEntries.h"
#include "ISF_FatrasDetDescrTools/PlanarDetLayerBuilder.h"
#include "ISF_FatrasDetDescrTools/InputLayerMaterialProvider.h"
#include "ISF_FatrasDetDescrTools/LayerMaterialProvider.h"

using namespace iFatras;

DECLARE_TOOL_FACTORY( PlanarDetLayerBuilder )
DECLARE_TOOL_FACTORY( InputLayerMaterialProvider )
DECLARE_TOOL_FACTORY( LayerMaterialProvider )

/** factory entries need to have the name of the package */
DECLARE_FACTORY_ENTRIES( ISF_FatrasDetDescrTools )
{
  DECLARE_TOOL( PlanarDetLayerBuilder )
  DECLARE_TOOL( InputLayerMaterialProvider )
  DECLARE_TOOL( LayerMaterialProvider )
}
