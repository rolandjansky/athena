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



DECLARE_ALGORITHM_FACTORY( CaloTowerMonitor )
DECLARE_ALGORITHM_FACTORY( CaloTowerAlgorithm )
DECLARE_ALGORITHM_FACTORY( CaloTopoTowerAlgorithm )
DECLARE_ALGORITHM_FACTORY( CaloClusterMaker )
DECLARE_ALGORITHM_FACTORY( CaloCell2ClusterMapper )
DECLARE_ALGORITHM_FACTORY( CaloCellMaker )
DECLARE_ALGORITHM_FACTORY( CaloConstCellMaker )

//DECLARE_ALGORITHM_FACTORY( CaloClusterCellSlimmer )
DECLARE_ALGORITHM_FACTORY( CaloClusterCorrDBWriter )
DECLARE_ALGORITHM_FACTORY( CaloClusterCorrDumper )
DECLARE_ALGORITHM_FACTORY( CaloTopoTowerAlg )
DECLARE_ALGORITHM_FACTORY( CaloCellOverwrite )
DECLARE_ALGORITHM_FACTORY( CaloClusterCellLinksUpdater )

DECLARE_ALGORITHM_FACTORY( CaloTowerxAODFromCells )
DECLARE_ALGORITHM_FACTORY( CaloTowerxAODFromClusters )

DECLARE_TOOL_FACTORY( CaloTopoClusterMaker )
DECLARE_TOOL_FACTORY( CaloTopoClusterSplitter )
DECLARE_TOOL_FACTORY( CaloClusterCopier )
DECLARE_TOOL_FACTORY( CaloClusterBuilderSW )
DECLARE_TOOL_FACTORY( CaloClusterMomentsMaker )
DECLARE_TOOL_FACTORY( CaloCellContainerMergerTool )
DECLARE_TOOL_FACTORY( CaloCellContainerCorrectorTool )
DECLARE_TOOL_FACTORY( CaloCellContainerCheckerTool )
DECLARE_TOOL_FACTORY( CaloCellContainerFinalizerTool )
DECLARE_TOOL_FACTORY( CaloCellContainerFromClusterTool )
//DECLARE_TOOL_FACTORY( CaloClusterPrinter )
//DECLARE_TOOL_FACTORY( CaloClusterLockVars )
DECLARE_TOOL_FACTORY ( Blob2ToolConstants )
DECLARE_TOOL_FACTORY ( CaloCellFastCopyTool )
DECLARE_TOOL_FACTORY ( CaloClusterSnapshot )

