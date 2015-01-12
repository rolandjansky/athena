#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrkMaterialProvider/TrkMaterialProviderTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY( Trk, TrkMaterialProviderTool )

DECLARE_FACTORY_ENTRIES( TrkMaterialProvider )
{
  DECLARE_NAMESPACE_TOOL( Trk, TrkMaterialProviderTool )
}


