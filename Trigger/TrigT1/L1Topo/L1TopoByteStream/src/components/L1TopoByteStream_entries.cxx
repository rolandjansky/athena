#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../L1TopoByteStreamCnv.h"
#include "../L1TopoByteStreamTool.h"

DECLARE_CONVERTER_FACTORY( L1TopoByteStreamCnv )
DECLARE_TOOL_FACTORY( L1TopoByteStreamTool )

DECLARE_FACTORY_ENTRIES( L1TopoByteStream)
{
  DECLARE_CONVERTER( L1TopoByteStreamCnv )
  DECLARE_TOOL( L1TopoByteStreamTool )

}
