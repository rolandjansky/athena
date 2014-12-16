#include "GoodRunsLists/GoodRunsListSelectorTool.h"
#include "GoodRunsLists/TriggerRegistryTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GoodRunsLists/GoodRunsListSelectionTool.h"

DECLARE_TOOL_FACTORY( GoodRunsListSelectorTool )
DECLARE_TOOL_FACTORY( GoodRunsListSelectionTool )
DECLARE_TOOL_FACTORY( TriggerRegistryTool )

DECLARE_FACTORY_ENTRIES( GoodRunsLists ) {
    //DECLARE_ALGTOOL( GoodRunsListSelectorTool )
    DECLARE_TOOL( GoodRunsListSelectorTool )
    DECLARE_TOOL( GoodRunsListSelectionTool )
    DECLARE_ALGTOOL( TriggerRegistryTool )
}

