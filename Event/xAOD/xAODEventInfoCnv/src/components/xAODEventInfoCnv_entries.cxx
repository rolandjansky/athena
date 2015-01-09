// $Id: xAODEventInfoCnv_entries.cxx 638573 2015-01-09 16:15:16Z cranshaw $

// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "../EventInfoCnvTool.h"
#include "../EventInfoSelectorTool.h"
#include "../EventInfoCnvAlg.h"
#include "../EventInfoReaderAlg.h"

DECLARE_NAMESPACE_TOOL_FACTORY( xAODMaker, EventInfoCnvTool )
DECLARE_NAMESPACE_TOOL_FACTORY( xAODMaker, EventInfoSelectorTool )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODMaker, EventInfoCnvAlg )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODReader, EventInfoReaderAlg )

DECLARE_FACTORY_ENTRIES( xAODEventInfoCnv ) {

   DECLARE_NAMESPACE_TOOL( xAODMaker, EventInfoCnvTool )
   DECLARE_NAMESPACE_TOOL( xAODMaker, EventInfoSelectorTool )
   DECLARE_NAMESPACE_ALGORITHM( xAODMaker, EventInfoCnvAlg )
   DECLARE_NAMESPACE_ALGORITHM( xAODReader, EventInfoReaderAlg )

}
