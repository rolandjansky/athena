#include "GoodRunsLists/GoodRunsListSelectorTool.h"
#include "GoodRunsLists/TriggerRegistryTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GoodRunsLists/GoodRunsListSelectionTool.h"
#include "GoodRunsLists/GRLSelectorAlg.h"

#ifndef XAOD_ANALYSIS
DECLARE_TOOL_FACTORY( GoodRunsListSelectorTool )
#endif // XAOD_ANALYSIS

DECLARE_TOOL_FACTORY( GoodRunsListSelectionTool )
DECLARE_TOOL_FACTORY( TriggerRegistryTool )


DECLARE_ALGORITHM_FACTORY( GRLSelectorAlg )

DECLARE_FACTORY_ENTRIES( GoodRunsLists ) {
  DECLARE_ALGORITHM( GRLSelectorAlg );
    //DECLARE_ALGTOOL( GoodRunsListSelectorTool )
#ifndef XAOD_ANALYSIS
    DECLARE_TOOL( GoodRunsListSelectorTool )
#endif // XAOD_ANALYSIS
    DECLARE_TOOL( GoodRunsListSelectionTool )
    DECLARE_ALGTOOL( TriggerRegistryTool )
}

