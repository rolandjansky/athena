// $Id: xAODEventInfoCnv_entries.cxx 663741 2015-04-29 11:52:21Z krasznaa $

// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "../EventInfoCnvTool.h"
#include "../EventInfoSelectorTool.h"
#include "../EventInfoCnvAlg.h"
#include "../EventInfoReaderAlg.h"
#include "../EventDuplicateFinderAlg.h"

DECLARE_NAMESPACE_TOOL_FACTORY( xAODMaker, EventInfoCnvTool )
DECLARE_NAMESPACE_TOOL_FACTORY( xAODMaker, EventInfoSelectorTool )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODMaker, EventInfoCnvAlg )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODReader, EventInfoReaderAlg )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODReader, EventDuplicateFinderAlg )

DECLARE_FACTORY_ENTRIES( xAODEventInfoCnv ) {

   DECLARE_NAMESPACE_TOOL( xAODMaker, EventInfoCnvTool )
   DECLARE_NAMESPACE_TOOL( xAODMaker, EventInfoSelectorTool )
   DECLARE_NAMESPACE_ALGORITHM( xAODMaker, EventInfoCnvAlg )
   DECLARE_NAMESPACE_ALGORITHM( xAODReader, EventInfoReaderAlg )
   DECLARE_NAMESPACE_ALGORITHM( xAODReader, EventDuplicateFinderAlg )

}
