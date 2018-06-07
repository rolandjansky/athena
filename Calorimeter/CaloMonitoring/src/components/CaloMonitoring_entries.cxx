//#include "CaloMonitoring/CaloCellMonitoringTool.h"
#include "CaloMonitoring/CaloCellVecMon.h"
//#include "CaloMonitoring/CaloCellTimeMon.h"
//#include "CaloMonitoring/CaloClusterMon.h"
#include "CaloMonitoring/CaloClusterVecMon.h"
//#include "CaloMonitoring/CaloClusterShwrMon.h"
//#include "CaloMonitoring/CaloClusterTimeMon.h"
//#include "CaloMonitoring/CaloCosmicsClusterMon.h"
#include "CaloMonitoring/CaloTowerVecMon.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "CaloMonitoring/CaloTransverseBalanceVecMon.h"
//DECLARE_TOOL_FACTORY(CaloCellMonitoringTool )
DECLARE_TOOL_FACTORY(CaloCellVecMon)
//DECLARE_TOOL_FACTORY(CaloCellTimeMon)
//DECLARE_TOOL_FACTORY(CaloClusterMon)
DECLARE_TOOL_FACTORY(CaloClusterVecMon)
  //DECLARE_TOOL_FACTORY(CaloClusterShwrMon)
  //DECLARE_TOOL_FACTORY(CaloClusterTimeMon)
//DECLARE_TOOL_FACTORY(CaloCosmicsClusterMon)
DECLARE_TOOL_FACTORY(CaloTowerVecMon)

DECLARE_TOOL_FACTORY(CaloTransverseBalanceVecMon)

DECLARE_FACTORY_ENTRIES(CaloMonitoring) {
  //  DECLARE_ALGTOOL(CaloCellMonitoringTool )
  DECLARE_ALGTOOL(CaloCellVecMon)
    //  DECLARE_ALGTOOL(CaloCellTimeMon)
    //  DECLARE_ALGTOOL(CaloClusterMon)
  DECLARE_ALGTOOL(CaloClusterVecMon)
    //  DECLARE_ALGTOOL(CaloClusterShwrMon)
    //  DECLARE_ALGTOOL(CaloClusterTimeMon)
    // DECLARE_ALGTOOL(CaloCosmicsClusterMon)
  DECLARE_ALGTOOL(CaloTowerVecMon)
  DECLARE_ALGTOOL(CaloTransverseBalanceVecMon)
}
