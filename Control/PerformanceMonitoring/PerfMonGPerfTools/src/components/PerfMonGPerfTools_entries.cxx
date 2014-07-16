// $Id: PerfMonGPerfTools_entries.cxx 496380 2012-04-18 12:28:09Z ritsch $

// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "../ProfilerService.h"

DECLARE_NAMESPACE_SERVICE_FACTORY( GPT, ProfilerService )

DECLARE_FACTORY_ENTRIES( PerfMonGPerfTools ) {

   DECLARE_NAMESPACE_SERVICE( GPT, ProfilerService )

}
