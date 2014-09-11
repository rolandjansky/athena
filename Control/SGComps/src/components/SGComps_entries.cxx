#include "../ProxyProviderSvc.h"
#include "../SGFolder.h"
#include "../SGDeleteAlg.h"
#include "../SGAccessAlg.h"
#include "../AddressRemappingSvc.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( SGDeleteAlg )
DECLARE_ALGORITHM_FACTORY( SGAccessAlg )
DECLARE_SERVICE_FACTORY( ProxyProviderSvc )
DECLARE_SERVICE_FACTORY( AddressRemappingSvc )
DECLARE_NAMESPACE_TOOL_FACTORY(SG, Folder )

DECLARE_FACTORY_ENTRIES(SGComps) {
    DECLARE_ALGORITHM( SGDeleteAlg )
    DECLARE_ALGORITHM( SGProxyAlg )
    DECLARE_SERVICE( ProxyProviderSvc )
    DECLARE_SERVICE( AddressRemappingSvc )
    DECLARE_NAMESPACE_TOOL( SG, Folder )
}
