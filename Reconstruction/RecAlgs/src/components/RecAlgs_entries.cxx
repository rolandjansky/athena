#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../TimingAlg.h"
#include "../MemoryAlg.h"
#include "../JobInfo.h"

DECLARE_ALGORITHM_FACTORY( TimingAlg )
DECLARE_ALGORITHM_FACTORY( MemoryAlg )
DECLARE_ALGORITHM_FACTORY( JobInfo )

DECLARE_FACTORY_ENTRIES( RecAlgs ) {
  DECLARE_ALGORITHM( TimingAlg );
  DECLARE_ALGORITHM( MemoryAlg );
  DECLARE_ALGORITHM( JobInfo );
}   
