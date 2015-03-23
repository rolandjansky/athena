// $Id: xAODEventFormatCnv_entries.cxx 655221 2015-03-18 21:27:44Z cranshaw $

// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "../EventFormatSvc.h"
#include "../EventFormatMetaDataTool.h"

DECLARE_NAMESPACE_SERVICE_FACTORY( xAODMaker, EventFormatSvc )
DECLARE_NAMESPACE_TOOL_FACTORY( xAODMaker, EventFormatMetaDataTool )

DECLARE_FACTORY_ENTRIES( xAODCreatorAlgs ) {

   DECLARE_NAMESPACE_SERVICE( xAODMaker, EventFormatSvc )
   DECLARE_NAMESPACE_TOOL( xAODMaker, EventFormatMetaDataTool )

}
