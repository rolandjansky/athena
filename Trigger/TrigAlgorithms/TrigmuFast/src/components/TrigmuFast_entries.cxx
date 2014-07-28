#include "TrigmuFast/muFast.h"
#include "TrigmuFast/PtLUTSvc.h"
#include "TrigmuFast/DataMonitor.h"
#include "TrigmuFast/L2DataMonitor.h"

#include "GaudiKernel/DeclareFactoryEntries.h"


DECLARE_ALGORITHM_FACTORY( muFast )
DECLARE_ALGORITHM_FACTORY( DataMonitor )
DECLARE_ALGORITHM_FACTORY( L2DataMonitor )

DECLARE_SERVICE_FACTORY( PtLUTSvc )
DECLARE_SERVICE_FACTORY( MuLUTSvc )
DECLARE_SERVICE_FACTORY( MuRoadsSvc )

DECLARE_FACTORY_ENTRIES(TrigmuFast) {
    DECLARE_SERVICE( MuRoadsSvc )
    DECLARE_SERVICE( MuLUTSvc )
    DECLARE_SERVICE( PtLUTSvc )
    DECLARE_ALGORITHM( muFast )
    DECLARE_ALGORITHM( DataMonitor )
    DECLARE_ALGORITHM( L2DataMonitor )
}
