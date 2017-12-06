#include "ISF_FatrasRecoTools/PlanarClusterOnTrackTool.h"
#include "ISF_FatrasRecoTools/PlanarSiSpacePointMakerTool.h"
#include "ISF_FatrasRecoTools/PlanarClusterProvider.h"
#include "ISF_FatrasRecoTools/ISF_SiSpacePointMakerTool.h"
#include "ISF_FatrasRecoTools/ISF_PRDGeneralTruthTrajectorySorterID.h"
#include "ISF_FatrasRecoTools/ISF_PRD_AssociationTool.h"
#include "ISF_FatrasRecoTools/ISF_TrackSummaryHelperTool.h"
#include "ISF_FatrasRecoTools/PRD_PlanarTruthTrajectoryBuilder.h"

using namespace iFatras;

DECLARE_COMPONENT( PlanarClusterOnTrackTool )
DECLARE_COMPONENT( PlanarSiSpacePointMakerTool )
DECLARE_COMPONENT( PlanarClusterProvider )
DECLARE_COMPONENT( ISF_SiSpacePointMakerTool )
DECLARE_COMPONENT( ISF_PRDGeneralTruthTrajectorySorterID )
DECLARE_COMPONENT( ISF_PRD_AssociationTool )
DECLARE_COMPONENT( ISF_TrackSummaryHelperTool )
DECLARE_COMPONENT( PRD_PlanarTruthTrajectoryBuilder )

