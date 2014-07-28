#include "GaudiKernel/DeclareFactoryEntries.h"
#include "RPCcabling/RPCcablingSvc.h"

DECLARE_SERVICE_FACTORY( RPCcablingSvc )

DECLARE_FACTORY_ENTRIES(RPCcabling) {
    DECLARE_SERVICE( RPCcablingSvc )
}
