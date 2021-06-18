
#include "GaudiKernel/DeclareFactoryEntries.h"


#include "TrigL0GepPerf/GepClusteringAlg.h"
DECLARE_ALGORITHM_FACTORY( GepClusteringAlg )

#include "TrigL0GepPerf/GepJetAlg.h"
DECLARE_ALGORITHM_FACTORY( GepJetAlg )

#include "TrigL0GepPerf/MissingETGepCl.h"
DECLARE_ALGORITHM_FACTORY( MissingETGepCl )

#include "TrigL0GepPerf/ClusterTiming.h"
DECLARE_ALGORITHM_FACTORY( ClusterTiming )

#include "TrigL0GepPerf/MissingETGepPufit.h"
DECLARE_ALGORITHM_FACTORY( MissingETGepPufit )

DECLARE_FACTORY_ENTRIES( TrigL0GepPerf ) 
{
  DECLARE_ALGORITHM( GepClusteringAlg );
  DECLARE_ALGORITHM( GepJetAlg );
  DECLARE_ALGORITHM( ClusterTiming );
  DECLARE_ALGORITHM( MissingETGepPufit );
  DECLARE_ALGORITHM( MissingETGepCl );
}
