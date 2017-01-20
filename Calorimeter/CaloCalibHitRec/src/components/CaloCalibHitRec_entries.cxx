#include "CaloCalibHitRec/CalibHitToCaloCell.h"
#include "CaloCalibHitRec/CalibHitToCaloCellTool.h"
#include "CaloCalibHitRec/CalibHitToCaloCellAlg.h"
#include "CaloCalibHitRec/TestTruthCellsAlg.h"
#include "CaloCalibHitRec/CalibHitIDCheck.h"
#include "CaloCalibHitRec/CaloCalibClusterMomentsMaker.h"
#include "CaloCalibHitRec/CaloCalibClusterMomentsMaker2.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( CalibHitToCaloCell )
DECLARE_ALGORITHM_FACTORY( CalibHitToCaloCellAlg )
DECLARE_ALGORITHM_FACTORY( CalibHitIDCheck )
DECLARE_ALGORITHM_FACTORY( TestTruthCellsAlg )

DECLARE_TOOL_FACTORY( CalibHitToCaloCellTool )
DECLARE_TOOL_FACTORY( CaloCalibClusterMomentsMaker )
DECLARE_TOOL_FACTORY( CaloCalibClusterMomentsMaker2 )

DECLARE_FACTORY_ENTRIES(CaloCalibHitRec) {
  DECLARE_ALGORITHM( CalibHitToCaloCell )
  DECLARE_ALGORITHM( CalibHitToCaloCellAlg )
  DECLARE_ALGORITHM( CalibHitIDCheck )
  DECLARE_ALGORITHM( TestTruthCellsAlg )
  DECLARE_TOOL( CalibHitToCaloCellTool )
  DECLARE_TOOL( CaloCalibClusterMomentsMaker )
  DECLARE_TOOL( CaloCalibClusterMomentsMaker2 )
}
