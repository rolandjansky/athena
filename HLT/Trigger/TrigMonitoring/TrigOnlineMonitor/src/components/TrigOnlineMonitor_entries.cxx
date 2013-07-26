#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrigOnlineMonitor/TrigROBMonitor.h"
#include "TrigOnlineMonitor/TrigMuCTPiROBMonitor.h"

DECLARE_ALGORITHM_FACTORY( TrigROBMonitor )
DECLARE_ALGORITHM_FACTORY( TrigMuCTPiROBMonitor )

DECLARE_FACTORY_ENTRIES(TrigOnlineMonitor) {
    DECLARE_ALGORITHM( TrigROBMonitor )
    DECLARE_ALGORITHM( TrigMuCTPiROBMonitor )
}
