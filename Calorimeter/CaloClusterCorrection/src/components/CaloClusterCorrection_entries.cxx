#include "CaloClusterCorrection/CaloClusterLocalCalib.h"
#include "CaloClusterCorrection/CaloClusterLocalCalibCone.h"
#include "CaloClusterCorrection/CaloClusterLogPos.h"
#include "CaloClusterCorrection/CaloFillRectangularCluster.h"
#include "CaloClusterCorrection/CaloClusterUpdate.h"
#include "CaloClusterCorrection/CaloTopoEMlayers.h"
#include "CaloClusterCorrection/CaloTopoEMphioff.h"
#include "CaloClusterCorrection/CaloTopoEMphimod.h"
#include "CaloClusterCorrection/CaloTopoEMsshape.h"
#include "CaloClusterCorrection/CaloTopoEMLongWeights.h"
#include "CaloClusterCorrection/CaloTopoEMGap.h"

#include "CaloClusterCorrection/CaloClusterCellWeightCalib.h"

#include "CaloClusterCorrection/CaloScaleCluster.h"
#include "CaloClusterCorrection/CaloSwApplyWgts_g3.h"
#include "CaloClusterCorrection/CaloSwEtaoff_v2.h"
#include "CaloClusterCorrection/CaloSwEtaoff_v3.h"
#include "CaloClusterCorrection/CaloSwPhioff_v2.h"
#include "CaloClusterCorrection/CaloSwEtamod_v2.h"
#include "CaloClusterCorrection/CaloSwPhimod_v2.h"
#include "CaloClusterCorrection/CaloSwGap_v2.h"
#include "CaloClusterCorrection/CaloSwGap_v3.h"
#include "CaloClusterCorrection/CaloSwClcon_v2.h"
#include "CaloClusterCorrection/CaloSwLongWeights.h"
#include "CaloClusterCorrection/CaloSwEtamod_g3.h"
#include "CaloClusterCorrection/CaloSwPhimod_g3.h"
#include "CaloClusterCorrection/CaloSwEta1b_g3.h"
#include "CaloClusterCorrection/CaloSwEta1e_g3.h"
#include "CaloClusterCorrection/CaloSwEta2b_g3.h"
#include "CaloClusterCorrection/CaloSwEta2e_g3.h"
#include "CaloClusterCorrection/CaloSwClcon_g3.h"
#include "CaloClusterCorrection/CaloSwGap_g3.h"
#include "CaloClusterCorrection/CaloSwECcrack_g3.h"
#include "CaloClusterCorrection/CaloComputeSWcellWeights.h"
#include "CaloClusterCorrection/CaloSwCalibHitsCalibration.h"
#include "CaloClusterCorrection/CaloSwDeadOTX_ps.h"
#include "CaloClusterCorrection/CaloSwDeadOTX_back.h"
#include "CaloClusterCorrection/CaloSwTime.h"
#include "CaloClusterCorrection/CaloSwTransitionRegionsCorr.h"

#include "CaloClusterCorrection/CaloLongWeights_v2.h"
#include "CaloClusterCorrection/CaloPhiParabola.h"


#include "CaloClusterCorrection/H1WeightToolCSC12Generic.h"

#include "CaloClusterCorrection/CaloClusterBadChannelList.h"
#include "CaloClusterCorrection/CaloClusterRemoveDuplicates.h"
#include "CaloClusterCorrection/CaloClusterRemoveBad.h"

#include "CaloClusterCorrection/CaloRunClusterCorrections.h"
#include "CaloClusterCorrection/CaloDummyCorrection.h"

#include "CaloClusterCorrection/CaloClusterTimeTool.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

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

DECLARE_FACTORY_ENTRIES(CaloClusterCorrection) {
    DECLARE_TOOL( CaloClusterLocalCalibCone )
    DECLARE_TOOL( CaloClusterLocalCalib )
    DECLARE_TOOL( CaloClusterLogPos )
    DECLARE_TOOL( CaloFillRectangularCluster )
    DECLARE_TOOL( CaloClusterUpdate )
    DECLARE_TOOL( CaloTopoEMlayers )
    DECLARE_TOOL( CaloTopoEMphioff )
    DECLARE_TOOL( CaloTopoEMphimod )
    DECLARE_TOOL( CaloTopoEMsshape )
    DECLARE_TOOL( CaloTopoEMLongWeights )
    DECLARE_TOOL( CaloTopoEMGap )

    DECLARE_TOOL( CaloClusterCellWeightCalib )

    DECLARE_TOOL( CaloSwApplyWgts_g3 )
    DECLARE_TOOL( CaloSwEtaoff_v2 )
    DECLARE_TOOL( CaloSwPhioff_v2 )
    DECLARE_TOOL( CaloSwEtamod_v2 )
    DECLARE_TOOL( CaloSwPhimod_v2 )
    DECLARE_TOOL( CaloSwGap_v2 )
    DECLARE_TOOL( CaloSwGap_v3 )
    DECLARE_TOOL( CaloSwClcon_v2 )
    DECLARE_TOOL( CaloSwLongWeights )
    DECLARE_TOOL( CaloSwEtamod_g3 )
    DECLARE_TOOL( CaloSwPhimod_g3 )
    DECLARE_TOOL( CaloSwEta1b_g3 )
    DECLARE_TOOL( CaloSwEta1e_g3 )
    DECLARE_TOOL( CaloSwEta2b_g3 )
    DECLARE_TOOL( CaloSwEta2e_g3 )
    DECLARE_TOOL( CaloSwClcon_g3 )
    DECLARE_TOOL( CaloSwGap_g3 )
    DECLARE_TOOL( CaloSwECcrack_g3 )
    DECLARE_TOOL( CaloComputeSWcellWeights )
    DECLARE_TOOL( CaloSwCalibHitsCalibration )
    DECLARE_TOOL(CaloSwDeadOTX_ps)
    DECLARE_TOOL(CaloSwDeadOTX_back)
    DECLARE_TOOL( CaloSwTime )
    DECLARE_TOOL( CaloSwTransitionRegionsCorr )
    
    DECLARE_TOOL( CaloLongWeights_v2 )
    DECLARE_TOOL( CaloPhiParabola )

    DECLARE_TOOL( H1WeightToolCSC12Generic )

    DECLARE_TOOL( CaloClusterBadChannelList )
    DECLARE_TOOL( CaloClusterRemoveDuplicates )
    DECLARE_TOOL( CaloClusterRemoveBad )

    DECLARE_TOOL( CaloRunClusterCorrections )
    DECLARE_TOOL( CaloDummyCorrection )

    DECLARE_ALGTOOL( CaloClusterTimeTool )
}
