#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/ActiveStoreSvc.h"
#include "StoreGate/tools/SGImplSvc.h"
#include "StoreGate/SegMemSvc.h"
#include "../SGHiveMgrSvc.h"

DECLARE_SERVICE_FACTORY( ActiveStoreSvc )
DECLARE_SERVICE_FACTORY( StoreGateSvc )
DECLARE_SERVICE_FACTORY( SGImplSvc )
DECLARE_SERVICE_FACTORY( SegMemSvc )
DECLARE_NAMESPACE_SERVICE_FACTORY( SG, HiveMgrSvc )

