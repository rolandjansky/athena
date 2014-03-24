// $Id: TrigConfxAOD_entries.cxx 580638 2014-01-29 13:18:16Z krasznaa $

// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "../xAODConfigSvc.h"
#include "../xAODMenuWriter.h"

DECLARE_NAMESPACE_SERVICE_FACTORY( TrigConf, xAODConfigSvc )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( TrigConf, xAODMenuWriter )

DECLARE_FACTORY_ENTRIES( TrigConfxAOD ) {

   DECLARE_NAMESPACE_SERVICE( TrigConf, xAODConfigSvc )
   DECLARE_NAMESPACE_ALGORITHM( TrigConf, xAODMenuWriter )

}
