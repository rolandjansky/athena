#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MuonCablingServers/RPCcablingServerSvc.h"
#include "MuonCablingServers/TGCcablingServerSvc.h"

DECLARE_SERVICE_FACTORY( RPCcablingServerSvc )
DECLARE_SERVICE_FACTORY( TGCcablingServerSvc )

DECLARE_FACTORY_ENTRIES(MuonCablingServers) {
    DECLARE_SERVICE( RPCcablingServerSvc );
    DECLARE_SERVICE( TGCcablingServerSvc );
}

