#include "TrigCaloRec/RoILArEMCellContMaker.h"
#include "TrigCaloRec/RoILArHadCellContMaker.h"
#include "TrigCaloRec/RoITileCellContMaker.h"
#include "TrigCaloRec/RoIFCalEmCellContMaker.h"
#include "TrigCaloRec/RoIFCalHadCellContMaker.h"
#include "TrigCaloRec/FullCaloCellContMaker.h"
#include "TrigCaloRec/TrigL1FCALTTSumFex.h"

#include "TrigCaloRec/TrigCaloClusterMaker.h"
#include "TrigCaloRec/TrigCaloCellMaker.h"
#include "TrigCaloRec/TrigCaloTowerMaker.h"
#include "TrigCaloRec/TrigFullCaloCellMaker.h"
#include "TrigCaloRec/TrigFullCaloClusterMaker.h"
#include "TrigCaloRec/TrigLArNoisyROAlg.h"
#include "TrigCaloRec/TrigL1BSTowerMaker.h"
#include "TrigCaloRec/TrigL1BSTowerHypo.h"

#include "TrigCaloRec/TrigCaloTopoTowerAlgorithm.h"
#include "TrigCaloRec/TrigCaloCell2ClusterMapper.h"

DECLARE_ALGORITHM_FACTORY( TrigCaloCellMaker )
DECLARE_ALGORITHM_FACTORY( TrigFullCaloCellMaker )
DECLARE_ALGORITHM_FACTORY( TrigCaloTowerMaker )
DECLARE_ALGORITHM_FACTORY( TrigCaloClusterMaker )
DECLARE_ALGORITHM_FACTORY( TrigCaloTopoTowerAlgorithm)
DECLARE_ALGORITHM_FACTORY( TrigCaloCell2ClusterMapper)
DECLARE_ALGORITHM_FACTORY( TrigFullCaloClusterMaker )
DECLARE_ALGORITHM_FACTORY( TrigLArNoisyROAlg )
DECLARE_ALGORITHM_FACTORY( TrigL1BSTowerMaker )
DECLARE_ALGORITHM_FACTORY( TrigL1BSTowerHypo )
DECLARE_ALGORITHM_FACTORY( TrigL1FCALTTSumFex )
DECLARE_TOOL_FACTORY( RoILArEMCellContMaker )
DECLARE_TOOL_FACTORY( RoILArHadCellContMaker )
DECLARE_TOOL_FACTORY( RoITileCellContMaker )
DECLARE_TOOL_FACTORY( RoIFCalEmCellContMaker )
DECLARE_TOOL_FACTORY( RoIFCalHadCellContMaker )
DECLARE_TOOL_FACTORY( FullCaloCellContMaker )

