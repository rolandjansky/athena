#include "CaloClusterCorrection/CaloClusterLocalCalib.h"
#include "../CaloClusterLocalCalibCone.h"
#include "../CaloClusterLogPos.h"
#include "CaloClusterCorrection/CaloFillRectangularCluster.h"
#include "../CaloClusterUpdate.h"
#include "../CaloTopoEMlayers.h"
#include "../CaloTopoEMphioff.h"
#include "../CaloTopoEMphimod.h"
#include "../CaloTopoEMsshape.h"
#include "../CaloTopoEMLongWeights.h"
#include "../CaloTopoEMGap.h"

#include "../CaloClusterCellWeightCalib.h"

#include "../CaloScaleCluster.h"
#include "../CaloSwApplyWgts_g3.h"
#include "../CaloSwEtaoff_v2.h"
#include "../CaloSwEtaoff_v3.h"
#include "../CaloSwPhioff_v2.h"
#include "../CaloSwEtamod_v2.h"
#include "../CaloSwPhimod_v2.h"
#include "../CaloSwGap_v2.h"
#include "../CaloSwGap_v3.h"
#include "../CaloSwClcon_v2.h"
#include "../CaloSwLongWeights.h"
#include "../CaloSwEtamod_g3.h"
#include "../CaloSwPhimod_g3.h"
#include "../CaloSwEta1b_g3.h"
#include "../CaloSwEta1e_g3.h"
#include "../CaloSwEta2b_g3.h"
#include "../CaloSwEta2e_g3.h"
#include "../CaloSwClcon_g3.h"
#include "../CaloSwGap_g3.h"
#include "../CaloSwECcrack_g3.h"
#include "../CaloComputeSWcellWeights.h"
#include "../CaloSwCalibHitsCalibration.h"
#include "../CaloSwDeadOTX_ps.h"
#include "../CaloSwDeadOTX_back.h"
#include "../CaloSwTime.h"
#include "../CaloSwTransitionRegionsCorr.h"

#include "../CaloLongWeights_v2.h"
#include "../CaloPhiParabola.h"


#include "../H1WeightToolCSC12Generic.h"

#include "../CaloClusterBadChannelList.h"
#include "../CaloClusterRemoveDuplicates.h"
#include "../CaloClusterRemoveBad.h"

#include "../CaloRunClusterCorrections.h"
#include "../CaloDummyCorrection.h"

#include "../CaloClusterTimeTool.h"


DECLARE_TOOL_FACTORY( CaloClusterLocalCalibCone )
DECLARE_TOOL_FACTORY( CaloClusterLocalCalib )
DECLARE_TOOL_FACTORY( CaloClusterLogPos )
DECLARE_TOOL_FACTORY( CaloFillRectangularCluster )
DECLARE_TOOL_FACTORY( CaloClusterUpdate )
DECLARE_TOOL_FACTORY( CaloTopoEMlayers )
DECLARE_TOOL_FACTORY( CaloTopoEMphioff )
DECLARE_TOOL_FACTORY( CaloTopoEMphimod )
DECLARE_TOOL_FACTORY( CaloTopoEMsshape )
DECLARE_TOOL_FACTORY( CaloTopoEMLongWeights )
DECLARE_TOOL_FACTORY( CaloTopoEMGap )

DECLARE_TOOL_FACTORY( CaloClusterCellWeightCalib )

DECLARE_TOOL_FACTORY( CaloScaleCluster )
DECLARE_TOOL_FACTORY( CaloSwApplyWgts_g3 )
DECLARE_TOOL_FACTORY( CaloSwEtaoff_v2 )
DECLARE_TOOL_FACTORY( CaloSwEtaoff_v3 )
DECLARE_TOOL_FACTORY( CaloSwPhioff_v2 )
DECLARE_TOOL_FACTORY( CaloSwEtamod_v2 )
DECLARE_TOOL_FACTORY( CaloSwPhimod_v2 )
DECLARE_TOOL_FACTORY( CaloSwGap_v2 )
DECLARE_TOOL_FACTORY( CaloSwGap_v3 )
DECLARE_TOOL_FACTORY( CaloSwClcon_v2 )
DECLARE_TOOL_FACTORY( CaloSwLongWeights )
DECLARE_TOOL_FACTORY( CaloSwEtamod_g3 )
DECLARE_TOOL_FACTORY( CaloSwPhimod_g3 )
DECLARE_TOOL_FACTORY( CaloSwEta1b_g3 )
DECLARE_TOOL_FACTORY( CaloSwEta1e_g3 )
DECLARE_TOOL_FACTORY( CaloSwEta2b_g3 )
DECLARE_TOOL_FACTORY( CaloSwEta2e_g3 )
DECLARE_TOOL_FACTORY( CaloSwClcon_g3 )
DECLARE_TOOL_FACTORY( CaloSwGap_g3 )
DECLARE_TOOL_FACTORY( CaloSwECcrack_g3 )
DECLARE_TOOL_FACTORY( CaloComputeSWcellWeights )
DECLARE_TOOL_FACTORY (CaloSwCalibHitsCalibration )
DECLARE_TOOL_FACTORY (CaloSwDeadOTX_ps)
DECLARE_TOOL_FACTORY (CaloSwDeadOTX_back)
 
DECLARE_TOOL_FACTORY (CaloSwTime )
DECLARE_TOOL_FACTORY (CaloSwTransitionRegionsCorr )

DECLARE_TOOL_FACTORY( CaloLongWeights_v2 )
DECLARE_TOOL_FACTORY( CaloPhiParabola )

DECLARE_TOOL_FACTORY (H1WeightToolCSC12Generic )

DECLARE_TOOL_FACTORY (CaloClusterBadChannelList )
DECLARE_TOOL_FACTORY (CaloClusterRemoveDuplicates )
DECLARE_TOOL_FACTORY (CaloClusterRemoveBad )

DECLARE_TOOL_FACTORY (CaloRunClusterCorrections )
DECLARE_TOOL_FACTORY (CaloDummyCorrection )

DECLARE_TOOL_FACTORY (CaloClusterTimeTool )

