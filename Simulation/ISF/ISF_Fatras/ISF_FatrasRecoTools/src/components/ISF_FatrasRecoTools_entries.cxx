#include "GaudiKernel/DeclareFactoryEntries.h"
#include "ISF_FatrasRecoTools/PlanarClusterOnTrackTool.h"
#include "ISF_FatrasRecoTools/PlanarSiSpacePointMakerTool.h"
#include "ISF_FatrasRecoTools/PlanarClusterProvider.h"
#include "ISF_FatrasRecoTools/ISF_SiSpacePointMakerTool.h"
#include "ISF_FatrasRecoTools/ISF_PRDGeneralTruthTrajectorySorterID.h"

using namespace iFatras;

DECLARE_TOOL_FACTORY( PlanarClusterOnTrackTool )
DECLARE_TOOL_FACTORY( PlanarSiSpacePointMakerTool )
DECLARE_TOOL_FACTORY( PlanarClusterProvider )
DECLARE_TOOL_FACTORY( ISF_SiSpacePointMakerTool )
DECLARE_TOOL_FACTORY( ISF_PRDGeneralTruthTrajectorySorterID )

DECLARE_FACTORY_ENTRIES( ISF_FatrasRecoTools ) 
{
  DECLARE_TOOL( PlanarClusterOnTrackTool )
  DECLARE_TOOL( PlanarSiSpacePointMakerTool )
  DECLARE_TOOL( PlanarClusterProvider )
  DECLARE_TOOL( ISF_SiSpacePointMakerTool )
  DECLARE_TOOL( ISF_PRDGeneralTruthTrajectorySorterID )
}
