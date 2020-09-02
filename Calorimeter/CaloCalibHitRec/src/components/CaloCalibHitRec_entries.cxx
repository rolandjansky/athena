#include "CaloCalibHitRec/CalibHitToCaloCell.h"
#include "CaloCalibHitRec/CalibHitIDCheck.h"
#include "CaloCalibHitRec/CaloCalibClusterMomentsMaker.h"
#include "CaloCalibHitRec/CaloCalibClusterMomentsMaker2.h"
#include "CaloCalibHitRec/CaloCalibClusterTruthAttributerTool.h"
#include "../CaloCalibClusterTruthMapMakerAlgorithm.h"
#include "../CaloCalibClusterDecoratorAlgorithm.h"
#include "CaloCalibHitRec/CalibHitToCaloCellTool.h"


DECLARE_COMPONENT( CalibHitToCaloCell )
DECLARE_COMPONENT( CalibHitIDCheck )

DECLARE_COMPONENT( CaloCalibClusterMomentsMaker )
DECLARE_COMPONENT( CaloCalibClusterMomentsMaker2 )
DECLARE_COMPONENT( CaloCalibClusterTruthAttributerTool )
DECLARE_COMPONENT( CaloCalibClusterTruthMapMakerAlgorithm )
DECLARE_COMPONENT( CaloCalibClusterDecoratorAlgorithm )
DECLARE_COMPONENT( CalibHitToCaloCellTool )


