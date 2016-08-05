#include "../TrigIDTPMonitor.h"
#include "../TrigIDTPMonitorElectron.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( TrigIDTPMonitor )
DECLARE_ALGORITHM_FACTORY( TrigIDTPMonitorElectron )

DECLARE_FACTORY_ENTRIES( TrigIDTPMonitor ) {
  DECLARE_ALGORITHM( TrigIDTPMonitor )
  DECLARE_ALGORITHM( TrigIDTPMonitorElectron )
}
