#include "TrigT1RPCRecRoiSvc/RPCRecRoiSvc.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_NAMESPACE_SERVICE_FACTORY  ( LVL1RPC, RPCRecRoiSvc )

DECLARE_FACTORY_ENTRIES(TrigT1RPCRecRoiSvc) {
        DECLARE_NAMESPACE_SERVICE   ( LVL1RPC, RPCRecRoiSvc )
}

