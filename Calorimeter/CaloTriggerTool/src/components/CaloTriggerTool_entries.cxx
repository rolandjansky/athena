#include "GaudiKernel/DeclareFactoryEntries.h"

#include "CaloTriggerTool/CaloTriggerTowerService.h"
#include "../CaloSuperCellIDTool.h"

DECLARE_TOOL_FACTORY( CaloTriggerTowerService )
DECLARE_TOOL_FACTORY( CaloSuperCellIDTool )

DECLARE_FACTORY_ENTRIES(CaloTriggerTool) {
  DECLARE_TOOL( CaloTriggerTowerService )
  DECLARE_TOOL( CaloSuperCellIDTool )
}

