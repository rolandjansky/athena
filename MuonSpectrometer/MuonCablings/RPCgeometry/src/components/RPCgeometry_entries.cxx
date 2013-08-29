#include "GaudiKernel/DeclareFactoryEntries.h"
#include "RPCgeometry/RPCgeometrySvc.h"

DECLARE_SERVICE_FACTORY( RPCgeometrySvc )

DECLARE_FACTORY_ENTRIES(RPCgeometry) {
    DECLARE_SERVICE( RPCgeometrySvc )
}
