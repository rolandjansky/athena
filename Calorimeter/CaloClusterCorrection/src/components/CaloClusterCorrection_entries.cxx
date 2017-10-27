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


DECLARE_COMPONENT( CaloClusterLocalCalibCone )
DECLARE_COMPONENT( CaloClusterLocalCalib )
DECLARE_COMPONENT( CaloClusterLogPos )
DECLARE_COMPONENT( CaloFillRectangularCluster )
DECLARE_COMPONENT( CaloClusterUpdate )
DECLARE_COMPONENT( CaloTopoEMlayers )
DECLARE_COMPONENT( CaloTopoEMphioff )
DECLARE_COMPONENT( CaloTopoEMphimod )
DECLARE_COMPONENT( CaloTopoEMsshape )
DECLARE_COMPONENT( CaloTopoEMLongWeights )
DECLARE_COMPONENT( CaloTopoEMGap )

DECLARE_COMPONENT( CaloClusterCellWeightCalib )

DECLARE_COMPONENT( CaloScaleCluster )
DECLARE_COMPONENT( CaloSwApplyWgts_g3 )
DECLARE_COMPONENT( CaloSwEtaoff_v2 )
DECLARE_COMPONENT( CaloSwEtaoff_v3 )
DECLARE_COMPONENT( CaloSwPhioff_v2 )
DECLARE_COMPONENT( CaloSwEtamod_v2 )
DECLARE_COMPONENT( CaloSwPhimod_v2 )
DECLARE_COMPONENT( CaloSwGap_v2 )
DECLARE_COMPONENT( CaloSwGap_v3 )
DECLARE_COMPONENT( CaloSwClcon_v2 )
DECLARE_COMPONENT( CaloSwLongWeights )
DECLARE_COMPONENT( CaloSwEtamod_g3 )
DECLARE_COMPONENT( CaloSwPhimod_g3 )
DECLARE_COMPONENT( CaloSwEta1b_g3 )
DECLARE_COMPONENT( CaloSwEta1e_g3 )
DECLARE_COMPONENT( CaloSwEta2b_g3 )
DECLARE_COMPONENT( CaloSwEta2e_g3 )
DECLARE_COMPONENT( CaloSwClcon_g3 )
DECLARE_COMPONENT( CaloSwGap_g3 )
DECLARE_COMPONENT( CaloSwECcrack_g3 )
DECLARE_COMPONENT( CaloComputeSWcellWeights )
DECLARE_COMPONENT( CaloSwCalibHitsCalibration )
DECLARE_COMPONENT( CaloSwDeadOTX_ps )
DECLARE_COMPONENT( CaloSwDeadOTX_back )
 
DECLARE_COMPONENT( CaloSwTime )
DECLARE_COMPONENT( CaloSwTransitionRegionsCorr )

DECLARE_COMPONENT( CaloLongWeights_v2 )
DECLARE_COMPONENT( CaloPhiParabola )

DECLARE_COMPONENT( H1WeightToolCSC12Generic )

DECLARE_COMPONENT( CaloClusterBadChannelList )
DECLARE_COMPONENT( CaloClusterRemoveDuplicates )
DECLARE_COMPONENT( CaloClusterRemoveBad )

DECLARE_COMPONENT( CaloRunClusterCorrections )
DECLARE_COMPONENT( CaloDummyCorrection )

DECLARE_COMPONENT( CaloClusterTimeTool )

