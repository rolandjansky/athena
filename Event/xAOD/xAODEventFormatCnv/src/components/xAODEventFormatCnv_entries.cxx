// $Id: xAODEventFormatCnv_entries.cxx 578438 2014-01-15 09:34:19Z krasznaa $

// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "../EventFormatSvc.h"

DECLARE_NAMESPACE_SERVICE_FACTORY( xAODMaker, EventFormatSvc )

DECLARE_FACTORY_ENTRIES( xAODCreatorAlgs ) {

   DECLARE_NAMESPACE_SERVICE( xAODMaker, EventFormatSvc )

}
