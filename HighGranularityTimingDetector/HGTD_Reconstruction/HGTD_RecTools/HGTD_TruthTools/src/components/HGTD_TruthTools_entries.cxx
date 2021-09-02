#include "GaudiKernel/DeclareFactoryEntries.h"
#include "HGTD_TruthTools/ClusterTruthTool.h"

using namespace HGTD;

DECLARE_TOOL_FACTORY(ClusterTruthTool)

DECLARE_FACTORY_ENTRIES( HGTD_TruthTools )
{
  DECLARE_TOOL( ClusterTruthTool )
}
