#include "../CaloTowerMonitor.h"
#include "../CaloTowerAlgorithm.h"
#include "../CaloTopoTowerAlgorithm.h"
#include "../CaloClusterMaker.h"
#include "../CaloTopoClusterMaker.h"
#include "../CaloTopoClusterSplitter.h"
#include "../CaloClusterCopier.h"
#include "../CaloClusterBuilderSW.h"
#include "../CaloClusterMomentsMaker.h"
#include "../CaloCell2ClusterMapper.h"
#include "../CaloCellContainerMergerTool.h"
#include "../CaloCellContainerCorrectorTool.h"
#include "../CaloCellContainerCheckerTool.h"
#include "../CaloCellContainerFinalizerTool.h"
#include "../CaloCellContainerFromClusterTool.h"
#include "../CaloCellMaker.h"
#include "../CaloConstCellMaker.h"
//#include "CaloRec/CaloClusterCellSlimmer.h" Really obsolete AFAIK (commissioning legacy)
#include "../CaloClusterCorrDBWriter.h"
#include "../CaloClusterCorrDumper.h"
//#include "CaloRec/CaloClusterPrinter.h"
//#include "CaloRec/CaloClusterLockVars.h"
#include "../CaloTopoTowerAlg.h"
#include "CaloRec/Blob2ToolConstants.h"
#include "../CaloCellOverwrite.h"
#include "../CaloCellFastCopyTool.h"
#include "../CaloClusterCellLinksUpdater.h"
#include "../CaloTowerxAODFromCells.h"
#include "../CaloTowerxAODFromClusters.h"
#include "../CaloClusterSnapshot.h"



DECLARE_COMPONENT( CaloTowerMonitor )
DECLARE_COMPONENT( CaloTowerAlgorithm )
DECLARE_COMPONENT( CaloTopoTowerAlgorithm )
DECLARE_COMPONENT( CaloClusterMaker )
DECLARE_COMPONENT( CaloCell2ClusterMapper )
DECLARE_COMPONENT( CaloCellMaker )
DECLARE_COMPONENT( CaloConstCellMaker )

//DECLARE_COMPONENT( CaloClusterCellSlimmer )
DECLARE_COMPONENT( CaloClusterCorrDBWriter )
DECLARE_COMPONENT( CaloClusterCorrDumper )
DECLARE_COMPONENT( CaloTopoTowerAlg )
DECLARE_COMPONENT( CaloCellOverwrite )
DECLARE_COMPONENT( CaloClusterCellLinksUpdater )

DECLARE_COMPONENT( CaloTowerxAODFromCells )
DECLARE_COMPONENT( CaloTowerxAODFromClusters )

DECLARE_COMPONENT( CaloTopoClusterMaker )
DECLARE_COMPONENT( CaloTopoClusterSplitter )
DECLARE_COMPONENT( CaloClusterCopier )
DECLARE_COMPONENT( CaloClusterBuilderSW )
DECLARE_COMPONENT( CaloClusterMomentsMaker )
DECLARE_COMPONENT( CaloCellContainerMergerTool )
DECLARE_COMPONENT( CaloCellContainerCorrectorTool )
DECLARE_COMPONENT( CaloCellContainerCheckerTool )
DECLARE_COMPONENT( CaloCellContainerFinalizerTool )
DECLARE_COMPONENT( CaloCellContainerFromClusterTool )
//DECLARE_COMPONENT( CaloClusterPrinter )
//DECLARE_COMPONENT( CaloClusterLockVars )
DECLARE_COMPONENT( Blob2ToolConstants )
DECLARE_COMPONENT( CaloCellFastCopyTool )
DECLARE_COMPONENT( CaloClusterSnapshot )

