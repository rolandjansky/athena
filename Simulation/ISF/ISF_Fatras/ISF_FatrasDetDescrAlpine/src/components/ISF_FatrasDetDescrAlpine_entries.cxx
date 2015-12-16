#include "GaudiKernel/DeclareFactoryEntries.h"
#include "ISF_FatrasDetDescrAlpine/PlanarModuleProvider.h"

using namespace iFatras;

DECLARE_TOOL_FACTORY( PlanarModuleProvider )

// factory entries need to have the name of the package
DECLARE_FACTORY_ENTRIES( ISF_FatrasDetDescrAlpine )
{
  DECLARE_TOOL( PlanarModuleProvider )
}
