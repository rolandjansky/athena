#include "GaudiKernel/DeclareFactoryEntries.h"

#include "CaloMonitoring/LArCellMonTool.h"
#include "CaloMonitoring/TileCalCellMonTool.h"
#include "CaloMonitoring/CaloClusterVecMon.h"
#include "CaloMonitoring/CaloTowerVecMon.h"
#include "CaloMonitoring/CaloTransverseBalanceVecMon.h"
#include "CaloMonitoring/CaloBaselineMon.h"

DECLARE_TOOL_FACTORY(TileCalCellMonTool)
DECLARE_TOOL_FACTORY(LArCellMonTool)
DECLARE_TOOL_FACTORY(CaloClusterVecMon)
DECLARE_TOOL_FACTORY(CaloTowerVecMon)
DECLARE_TOOL_FACTORY(CaloTransverseBalanceVecMon)
DECLARE_TOOL_FACTORY(CaloBaselineMon)

