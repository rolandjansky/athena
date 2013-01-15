
// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"


// Declared service(s):
#include "../MergingEventLoopMgr.h"

DECLARE_NAMESPACE_SERVICE_FACTORY(TrigSim, MergingEventLoopMgr)

/*
DECLARE_FACTORY_ENTRIES(TrigOfflineValidation) {
    DECLARE_NAMESPACE_SERVICE(TrigVal, TwoInputEventLoopMgr)
}
*/

