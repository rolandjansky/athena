// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "../MBTSModuleCnvAlg.h"
#include "../MBTSModuleCnvTool.h"
#include "../ForwardEventInfoCnvAlg.h"
#include "../ForwardEventInfoCnvTool.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODMaker, MBTSModuleCnvAlg )
DECLARE_NAMESPACE_TOOL_FACTORY( xAODMaker, MBTSModuleCnvTool )

DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODMaker, ForwardEventInfoCnvAlg )
DECLARE_NAMESPACE_TOOL_FACTORY( xAODMaker, ForwardEventInfoCnvTool )

DECLARE_FACTORY_ENTRIES( xAODForwardCnv ) {
   DECLARE_NAMESPACE_ALGORITHM( xAODMaker, MBTSModuleCnvAlg )
   DECLARE_NAMESPACE_TOOL( xAODMaker, MBTSModuleCnvTool )
   DECLARE_NAMESPACE_ALGORITHM( xAODMaker, ForwardEventInfoCnvAlg )
   DECLARE_NAMESPACE_TOOL( xAODMaker, ForwardEventInfoCnvTool )
}


