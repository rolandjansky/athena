//====================================================================

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "CaloDetDescr/CaloDepthTool.h"
#include "CaloDetDescr/CaloAlignTool.h"
#include "CaloDetDescr/CaloSuperCellAlignTool.h"

DECLARE_TOOL_FACTORY( CaloDepthTool )
DECLARE_TOOL_FACTORY( CaloAlignTool )
DECLARE_TOOL_FACTORY( CaloSuperCellAlignTool )

DECLARE_FACTORY_ENTRIES	( CaloDetDescr ) {
    DECLARE_TOOL( CaloDepthTool );
    DECLARE_TOOL( CaloAlignTool );
    DECLARE_TOOL( CaloSuperCellAlignTool );
}


