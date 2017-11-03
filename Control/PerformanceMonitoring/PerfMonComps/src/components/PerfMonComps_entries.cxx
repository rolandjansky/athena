#include "../PerfMonSvc.h"
// #include "../PerfMonLdTool.h"
#include "../CallGraphBuilderSvc.h"
#include "../CallGraphAuditor.h"
#include "../PerfMonAuditor.h"
#include "../PerfMonStorePayloadMon.h"

  
DECLARE_COMPONENT( PerfMonSvc )
DECLARE_COMPONENT( Athena::PerfMonAuditor )
// DECLARE_COMPONENT( PerfMon::LdTool )

DECLARE_COMPONENT( PerfMon::CallGraphBuilderSvc )
DECLARE_COMPONENT( PerfMon::CallGraphAuditor )
DECLARE_COMPONENT( PerfMon::StorePayloadMon )

