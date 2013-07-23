#include "GaudiKernel/DeclareFactoryEntries.h"

#include "ISProperty/ISPropertySvc.h"
#include "ISProperty/ISPropTestAlg.h"
#include "ISProperty/ISPropTestAlgTool.h"

// Declaration of the AlgTool Factory

DECLARE_SERVICE_FACTORY( ISPropertySvc )
DECLARE_TOOL_FACTORY( ISPropTestAlgTool )
DECLARE_ALGORITHM_FACTORY( ISPropTestAlg )

DECLARE_FACTORY_ENTRIES( ISProperty ) {
    DECLARE_SERVICE    ( ISPropertySvc )
    DECLARE_ALGORITHM  ( ISPropTestAlg )
    DECLARE_TOOL       ( ISPropTestAlgTool )
}
