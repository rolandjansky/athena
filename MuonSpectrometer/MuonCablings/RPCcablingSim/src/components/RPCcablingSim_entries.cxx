#include "GaudiKernel/DeclareFactoryEntries.h"
#include "RPCcablingSim/RPCcablingSimSvc.h"

DECLARE_SERVICE_FACTORY( RPCcablingSimSvc )

DECLARE_FACTORY_ENTRIES(RPCcablingSim) {
    DECLARE_SERVICE( RPCcablingSimSvc );
}
