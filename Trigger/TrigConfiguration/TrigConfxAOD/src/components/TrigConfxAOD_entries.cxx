// $Id: TrigConfxAOD_entries.cxx 612011 2014-08-15 13:43:57Z krasznaa $

// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "../xAODConfigSvc.h"
#include "../xAODMenuWriter.h"
#include "../xAODMenuReader.h"
#include "TrigConfxAOD/xAODConfigTool.h"

DECLARE_NAMESPACE_SERVICE_FACTORY( TrigConf, xAODConfigSvc )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( TrigConf, xAODMenuWriter )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( TrigConf, xAODMenuReader )
DECLARE_NAMESPACE_TOOL_FACTORY( TrigConf, xAODConfigTool )

DECLARE_FACTORY_ENTRIES( TrigConfxAOD ) {

   DECLARE_NAMESPACE_SERVICE( TrigConf, xAODConfigSvc )
   DECLARE_NAMESPACE_ALGORITHM( TrigConf, xAODMenuWriter )
   DECLARE_NAMESPACE_ALGORITHM( TrigConf, xAODMenuReader )
   DECLARE_NAMESPACE_TOOL( TrigConf, xAODConfigTool )

}
