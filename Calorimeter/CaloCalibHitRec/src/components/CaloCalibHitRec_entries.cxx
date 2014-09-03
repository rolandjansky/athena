#include "CaloCalibHitRec/CalibHitToCaloCell.h"
#include "CaloCalibHitRec/CalibHitIDCheck.h"
#include "CaloCalibHitRec/CaloCalibClusterMomentsMaker.h"
#include "CaloCalibHitRec/CaloCalibClusterMomentsMaker2.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( CalibHitToCaloCell )
DECLARE_ALGORITHM_FACTORY( CalibHitIDCheck )

DECLARE_TOOL_FACTORY( CaloCalibClusterMomentsMaker )
DECLARE_TOOL_FACTORY( CaloCalibClusterMomentsMaker2 )

DECLARE_FACTORY_ENTRIES(CaloCalibHitRec) {
  DECLARE_ALGORITHM( CalibHitToCaloCell )
  DECLARE_ALGORITHM( CalibHitIDCheck )
  DECLARE_TOOL( CaloCalibClusterMomentsMaker )
  DECLARE_TOOL( CaloCalibClusterMomentsMaker2 )
}
