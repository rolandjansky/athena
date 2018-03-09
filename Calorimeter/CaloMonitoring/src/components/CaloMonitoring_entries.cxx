#include "CaloMonitoring/CaloCellVecMon.h"
#include "CaloMonitoring/CaloClusterVecMon.h"
#include "CaloMonitoring/CaloBaselineMon.h"
#include "CaloMonitoring/CaloTowerVecMon.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "CaloMonitoring/CaloTransverseBalanceVecMon.h"
DECLARE_TOOL_FACTORY(CaloCellVecMon)
DECLARE_TOOL_FACTORY(CaloClusterVecMon)
DECLARE_TOOL_FACTORY(CaloBaselineMon)
DECLARE_TOOL_FACTORY(CaloTowerVecMon)
DECLARE_TOOL_FACTORY(CaloTransverseBalanceVecMon)

DECLARE_FACTORY_ENTRIES(CaloMonitoring) {
  DECLARE_ALGTOOL(CaloCellVecMon)
  DECLARE_ALGTOOL(CaloClusterVecMon)
  DECLARE_ALGTOOL(CaloBaselineMon)
  DECLARE_ALGTOOL(CaloTowerVecMon)
  DECLARE_ALGTOOL(CaloTransverseBalanceVecMon)
}
