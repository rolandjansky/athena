#include "CaloRec/CaloTowerMonitor.h"
#include "CaloRec/CaloTowerMaker.h"
#include "CaloRec/CaloTowerAlgorithm.h"
#include "CaloRec/CaloTopoTowerAlgorithm.h"
#include "CaloRec/CaloClusterMaker.h"
#include "CaloRec/CaloTopoClusterMaker.h"
#include "CaloRec/CaloTopoClusterSplitter.h"
#include "CaloRec/CaloClusterCopier.h"
#include "CaloRec/CaloClusterBuilderSW.h"
#include "CaloRec/CaloClusterMomentsMaker.h"
#include "CaloRec/CaloClusterMomentsMaker_DigiHSTruth.h"
#include "CaloRec/CaloCell2ClusterMapper.h"
#include "CaloRec/CaloCellContainerMergerTool.h"
#include "CaloRec/CaloCellContainerCorrectorTool.h"
#include "CaloRec/CaloCellContainerCheckerTool.h"
#include "CaloRec/CaloCellContainerFinalizerTool.h"
#include "CaloRec/CaloCellContainerFromClusterTool.h"
#include "CaloRec/CaloCellMaker.h"
#include "CaloRec/CaloConstCellMaker.h"
//#include "CaloRec/CaloClusterCellSlimmer.h" Really obsolete AFAIK (commissioning legacy)
#include "CaloRec/CaloClusterCorrDBWriter.h"
#include "CaloRec/CaloClusterCorrDumper.h"
//#include "CaloRec/CaloClusterPrinter.h"
//#include "CaloRec/CaloClusterLockVars.h"
#include "CaloRec/CaloTopoTowerAlg.h"
#include "CaloRec/Blob2ToolConstants.h"
#include "CaloRec/CaloCellOverwrite.h"
#include "CaloRec/CaloCellFastCopyTool.h"
#include "CaloRec/CaloClusterCellLinksUpdater.h"
#include "CaloRec/CaloTowerxAODFromCells.h"
#include "CaloRec/CaloTowerxAODFromClusters.h"
#include "CaloRec/CaloClusterSnapshot.h"
#include "CaloRec/CaloTowerxAODAlgoBase.h"
#include "CaloRec/CaloTopoClusterFromTowerMaker.h"
#include "CaloRec/CaloTopoClusterFromTowerCalibrator.h"
#include "CaloRec/CaloTowerGeometrySvc.h"
 


#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( CaloTowerMonitor )
DECLARE_ALGORITHM_FACTORY( CaloTowerMaker )
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
DECLARE_ALGORITHM_FACTORY( CaloTowerxAODAlgoBase )
DECLARE_TOOL_FACTORY ( CaloTopoClusterFromTowerMaker )
DECLARE_TOOL_FACTORY ( CaloTopoClusterFromTowerCalibrator )
 

DECLARE_TOOL_FACTORY( CaloTopoClusterMaker )
DECLARE_TOOL_FACTORY( CaloTopoClusterSplitter )
DECLARE_TOOL_FACTORY( CaloClusterCopier )
DECLARE_TOOL_FACTORY( CaloClusterBuilderSW )
DECLARE_TOOL_FACTORY( CaloClusterMomentsMaker )
DECLARE_TOOL_FACTORY( CaloClusterMomentsMaker_DigiHSTruth )
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

DECLARE_FACTORY_ENTRIES(CaloRec) {
    DECLARE_ALGORITHM( CaloTowerMonitor );
    DECLARE_ALGORITHM( CaloTowerMaker );
    DECLARE_ALGORITHM( CaloTowerAlgorithm );
    DECLARE_ALGORITHM( CaloTopoTowerAlgorithm );
    DECLARE_ALGORITHM( CaloClusterMaker );
    DECLARE_ALGORITHM( CaloCell2ClusterMapper );
    DECLARE_ALGORITHM( CaloCellMaker );
    DECLARE_ALGORITHM( CaloConstCellMaker );
    //    DECLARE_ALGORITHM( CaloClusterCellSlimmer );
    DECLARE_ALGORITHM( CaloClusterCorrDBWriter )
    DECLARE_ALGORITHM( CaloClusterCorrDumper )
    DECLARE_ALGORITHM( CaloTopoTowerAlg )
    DECLARE_ALGORITHM( CaloCellOverwrite )
    DECLARE_ALGORITHM ( CaloClusterCellLinksUpdater )
    DECLARE_ALGORITHM( CaloTowerxAODAlgoBase )
    DECLARE_ALGORITHM( CaloTowerxAODFromCells )
    DECLARE_ALGORITHM( CaloTowerxAODFromClusters )
    DECLARE_TOOL( CaloTopoClusterFromTowerMaker )
    DECLARE_TOOL( CaloTopoClusterFromTowerCalibrator )


    DECLARE_TOOL( CaloTopoClusterMaker );
    DECLARE_TOOL( CaloTopoClusterSplitter );
    DECLARE_TOOL( CaloClusterBuilderSW );
    DECLARE_TOOL( CaloClusterCopier );
    DECLARE_TOOL( CaloClusterMomentsMaker );
    DECLARE_TOOL( CaloClusterMomentsMaker_DigiHSTruth );
    DECLARE_TOOL( CaloCellContainerMergerTool );
    DECLARE_TOOL( CaloCellContainerCorrectorTool );
    DECLARE_TOOL( CaloCellContainerCheckerTool );
    DECLARE_TOOL( CaloCellContainerFinalizerTool );
    //DECLARE_TOOL( CaloClusterPrinter );
    //DECLARE_TOOL( CaloClusterLockVars );
    DECLARE_TOOL( Blob2ToolConstants )
    DECLARE_TOOL( CaloCellFastCopyTool ) 
    DECLARE_TOOL( CaloClusterSnapshot )
}

DECLARE_COMPONENT( CaloTowerGeometrySvc )

