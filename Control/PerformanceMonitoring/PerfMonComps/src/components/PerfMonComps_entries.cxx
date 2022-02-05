#include "../PerfMonSvc.h"
// #include "../PerfMonLdTool.h"
#include "../CallGraphBuilderSvc.h"
#include "../CallGraphAuditor.h"
#include "../PerfMonAuditor.h"
#include "../PerfMonStorePayloadMon.h"

#include "../PerfMonMTSvc.h"
#include "../PerfMonMTAuditor.h"
  
DECLARE_COMPONENT( PerfMonSvc )
DECLARE_COMPONENT( Athena::PerfMonAuditor )
// DECLARE_COMPONENT( PerfMon::LdTool )

DECLARE_COMPONENT( PerfMon::CallGraphBuilderSvc )
DECLARE_COMPONENT( PerfMon::CallGraphAuditor )
DECLARE_COMPONENT( PerfMon::StorePayloadMon )

DECLARE_COMPONENT( PerfMonMTSvc )
DECLARE_COMPONENT( PerfMonMTAuditor )
