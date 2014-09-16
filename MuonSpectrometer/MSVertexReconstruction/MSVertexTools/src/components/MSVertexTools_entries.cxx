
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../MSVertexTrackletTool.h"
#include "../MSVertexRecoTool.h"

using namespace Muon;

DECLARE_TOOL_FACTORY( MSVertexTrackletTool )
DECLARE_TOOL_FACTORY( MSVertexRecoTool )

DECLARE_FACTORY_ENTRIES( MSVertexTools )
{
  DECLARE_TOOL( MSVertexTrackletTool )
  DECLARE_TOOL( MSVertexRecoTool )
}




