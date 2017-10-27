#include "CaloUtils/LocalNoiseSuppressionTool.h"
#include "CaloUtils/CaloCellWeightCorrection.h"
#include "CaloUtils/CaloLCClassificationTool.h"
#include "CaloUtils/EstimEMClusterClassificationTool.h"
#include "CaloUtils/CaloLCWeightTool.h"
#include "CaloUtils/CaloLCOutOfClusterTool.h"
#include "CaloUtils/CaloLCDeadMaterialTool.h"
#include "CaloUtils/CaloTopoTowerBuilderTool.h"
#include "CaloUtils/CaloTowerBuilderTool.h"
#include "CaloUtils/xAODClusterCompressor.h"
#include "../CaloTowerStoreTestAlg.h"
#include "../CaloTowerBuilderToolTestAlg.h"


DECLARE_COMPONENT( CaloCellWeightCorrection )
DECLARE_COMPONENT( LocalNoiseSuppressionTool )
DECLARE_COMPONENT( CaloLCClassificationTool )
DECLARE_COMPONENT( EstimEMClusterClassificationTool )
DECLARE_COMPONENT( CaloLCWeightTool )
DECLARE_COMPONENT( CaloLCOutOfClusterTool )
DECLARE_COMPONENT( CaloLCDeadMaterialTool )
DECLARE_COMPONENT( CaloTopoTowerBuilderTool )
DECLARE_COMPONENT( CaloTowerBuilderTool )
DECLARE_COMPONENT( xAODClusterCompressor )

DECLARE_COMPONENT( CaloTowerStoreTestAlg )
DECLARE_COMPONENT( CaloTowerBuilderToolTestAlg )

