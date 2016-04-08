// $Id: xAODMissingETCnv_entries.cxx 586557 2014-03-06 17:56:05Z krasznaa $

// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "../MissingETCnvTool.h"
#include "../MissingETCnvAlg.h"

DECLARE_NAMESPACE_TOOL_FACTORY( xAODMaker, MissingETCnvTool )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODMaker, MissingETCnvAlg )

DECLARE_FACTORY_ENTRIES( xAODMissingETCnv ) {

   DECLARE_NAMESPACE_TOOL( xAODMaker, MissingETCnvTool )
   DECLARE_NAMESPACE_ALGORITHM( xAODMaker, MissingETCnvAlg )

}
