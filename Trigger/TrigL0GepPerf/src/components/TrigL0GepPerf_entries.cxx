
#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../MissingETGepCl.h"
DECLARE_ALGORITHM_FACTORY( MissingETGepCl )

#include "../ClusterTiming.h"
DECLARE_ALGORITHM_FACTORY( ClusterTiming )

#include "../MissingETGepPufit.h"
DECLARE_ALGORITHM_FACTORY( MissingETGepPufit )

DECLARE_FACTORY_ENTRIES( TrigL0GepPerf ) 
{
  DECLARE_ALGORITHM( ClusterTiming );
  DECLARE_ALGORITHM( MissingETGepPufit );
  DECLARE_ALGORITHM( MissingETGepCl );
}
