#include "../MonitoredAlg.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( MonitoredAlg )

DECLARE_FACTORY_ENTRIES(AthExMonitored) {
    DECLARE_ALGORITHM( Monitored )
}
