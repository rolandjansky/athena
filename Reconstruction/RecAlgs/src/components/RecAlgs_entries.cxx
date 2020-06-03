#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../TimingAlg.h"
#include "../MemoryAlg.h"
#include "../JobInfo.h"
#include "../EventInfoUnlocker.h"
#include "../AppStopAlg.h"

DECLARE_ALGORITHM_FACTORY( TimingAlg )
DECLARE_ALGORITHM_FACTORY( MemoryAlg )
DECLARE_ALGORITHM_FACTORY( JobInfo )
DECLARE_ALGORITHM_FACTORY( EventInfoUnlocker )
DECLARE_ALGORITHM_FACTORY( AppStopAlg )

DECLARE_FACTORY_ENTRIES( RecAlgs ) {
  DECLARE_ALGORITHM( TimingAlg );
  DECLARE_ALGORITHM( MemoryAlg );
  DECLARE_ALGORITHM( JobInfo );
  DECLARE_ALGORITHM( EventInfoUnlocker );
  DECLARE_ALGORITHM( AppStopAlg );
}   
