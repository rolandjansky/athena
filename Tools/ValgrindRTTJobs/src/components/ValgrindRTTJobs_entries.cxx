#include "../ValgrindHelperAlg.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( ValgrindHelperAlg )

DECLARE_FACTORY_ENTRIES( ValgrindRTTJobs ) {
  DECLARE_ALGORITHM( ValgrindHelperAlg ) ;
}
