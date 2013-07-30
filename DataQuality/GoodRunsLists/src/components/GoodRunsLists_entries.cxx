#include "GoodRunsLists/GoodRunsListSelectorTool.h"
#include "GoodRunsLists/TriggerRegistryTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY( GoodRunsListSelectorTool )
DECLARE_TOOL_FACTORY( TriggerRegistryTool )

DECLARE_FACTORY_ENTRIES( GoodRunsLists ) {
    //DECLARE_ALGTOOL( GoodRunsListSelectorTool )
    DECLARE_TOOL( GoodRunsListSelectorTool )
    DECLARE_ALGTOOL( TriggerRegistryTool )
}

