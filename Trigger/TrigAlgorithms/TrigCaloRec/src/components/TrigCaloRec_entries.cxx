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

DECLARE_COMPONENT( TrigCaloCellMaker )
DECLARE_COMPONENT( TrigFullCaloCellMaker )
DECLARE_COMPONENT( TrigCaloTowerMaker )
DECLARE_COMPONENT( TrigCaloClusterMaker )
DECLARE_COMPONENT( TrigCaloTopoTowerAlgorithm )
DECLARE_COMPONENT( TrigCaloCell2ClusterMapper )
DECLARE_COMPONENT( TrigFullCaloClusterMaker )
DECLARE_COMPONENT( TrigLArNoisyROAlg )
DECLARE_COMPONENT( TrigL1BSTowerMaker )
DECLARE_COMPONENT( TrigL1BSTowerHypo )
DECLARE_COMPONENT( TrigL1FCALTTSumFex )
DECLARE_COMPONENT( RoILArEMCellContMaker )
DECLARE_COMPONENT( RoILArHadCellContMaker )
DECLARE_COMPONENT( RoITileCellContMaker )
DECLARE_COMPONENT( RoIFCalEmCellContMaker )
DECLARE_COMPONENT( RoIFCalHadCellContMaker )
DECLARE_COMPONENT( FullCaloCellContMaker )

