#include "GaudiKernel/DeclareFactoryEntries.h"
#include "InDetTestBLayer/InDetTestBLayerTool.h"


using namespace InDet ;

DECLARE_NAMESPACE_TOOL_FACTORY( InDet, InDetTestBLayerTool )


/** factory entries need to have the name of the package */
DECLARE_FACTORY_ENTRIES( InDetTestBLayer )
{
  DECLARE_NAMESPACE_TOOL( InDet, inDetTestBLayerTool );

}
