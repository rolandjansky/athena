// $Id: xAODTrigMissingETCnv_entries.cxx 592539 2014-04-11 10:17:29Z krasznaa $

// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "../TrigMissingETCnvTool.h"
#include "../TrigMissingETCnvAlg.h"

DECLARE_NAMESPACE_TOOL_FACTORY( xAODMaker, TrigMissingETCnvTool )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODMaker, TrigMissingETCnvAlg )

DECLARE_FACTORY_ENTRIES( xAODAnalysisTriggerCnv ) {

   DECLARE_NAMESPACE_TOOL( xAODMaker, TrigMissingETCnvTool )
   DECLARE_NAMESPACE_ALGORITHM( xAODMaker, TrigMissingETCnvAlg )
   DECLARE_NAMESPACE_TOOL( xAODMaker, TrigMissingETContainerCnvTool )
   DECLARE_NAMESPACE_ALGORITHM( xAODMaker, TrigMissingETContainerCnvAlg )

}
