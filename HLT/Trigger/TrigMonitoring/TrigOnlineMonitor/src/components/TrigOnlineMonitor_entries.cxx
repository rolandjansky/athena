#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrigOnlineMonitor/TrigROBMonitor.h"
#include "TrigOnlineMonitor/TrigMuCTPiROBMonitor.h"
#include "src/TrigALFAROBMonitor.h"
#include "src/TrigL1TopoWriteValData.h"

DECLARE_ALGORITHM_FACTORY( TrigROBMonitor )
DECLARE_ALGORITHM_FACTORY( TrigMuCTPiROBMonitor )
DECLARE_ALGORITHM_FACTORY( TrigALFAROBMonitor )
DECLARE_ALGORITHM_FACTORY( TrigL1TopoWriteValData )

DECLARE_FACTORY_ENTRIES(TrigOnlineMonitor) {
    DECLARE_ALGORITHM( TrigROBMonitor )
    DECLARE_ALGORITHM( TrigMuCTPiROBMonitor )
    DECLARE_ALGORITHM( TrigALFAROBMonitor )
    DECLARE_ALGORITHM( TrigL1TopoWriteValData )
}
