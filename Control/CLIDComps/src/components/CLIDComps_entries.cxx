#include "../ClassIDSvc.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_SERVICE_FACTORY( ClassIDSvc )

DECLARE_FACTORY_ENTRIES(CLIDComps) {
    DECLARE_SERVICE( ClassIDSvc )
}
