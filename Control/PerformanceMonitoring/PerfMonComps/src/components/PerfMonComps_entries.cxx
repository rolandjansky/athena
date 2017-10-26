#include "../PerfMonSvc.h"
// #include "../PerfMonLdTool.h"
#include "../CallGraphBuilderSvc.h"
#include "../CallGraphAuditor.h"
#include "../PerfMonAuditor.h"
#include "../PerfMonStorePayloadMon.h"

  
DECLARE_SERVICE_FACTORY( PerfMonSvc  )
DECLARE_NAMESPACE_AUDITOR_FACTORY( Athena, PerfMonAuditor )
// DECLARE_NAMESPACE_TOOL_FACTORY( PerfMon, LdTool )

DECLARE_NAMESPACE_SERVICE_FACTORY( PerfMon, CallGraphBuilderSvc )
DECLARE_NAMESPACE_AUDITOR_FACTORY( PerfMon, CallGraphAuditor )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( PerfMon, StorePayloadMon )

