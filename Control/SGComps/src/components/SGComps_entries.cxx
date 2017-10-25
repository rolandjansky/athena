#include "../ProxyProviderSvc.h"
#include "../SGFolder.h"
#include "../SGDeleteAlg.h"
#include "../SGAccessAlg.h"
#include "../AddressRemappingSvc.h"
#include "../SGCommitAuditor.h"
#include "../SGInputLoader.h"

DECLARE_ALGORITHM_FACTORY( SGDeleteAlg )
DECLARE_ALGORITHM_FACTORY( SGAccessAlg )
DECLARE_SERVICE_FACTORY( ProxyProviderSvc )
DECLARE_SERVICE_FACTORY( AddressRemappingSvc )
DECLARE_AUDITOR_FACTORY( SGCommitAuditor )
DECLARE_NAMESPACE_TOOL_FACTORY(SG, Folder )

DECLARE_ALGORITHM_FACTORY(SGInputLoader)

