#include "GaudiKernel/DeclareFactoryEntries.h"         

#include "../CaloRingerAlgorithm.h"
#include "../xAODRingSetConfWriter.h"

using namespace Ringer;

DECLARE_ALGORITHM_FACTORY(     CaloRingerAlgorithm      )
DECLARE_ALGORITHM_FACTORY(    xAODRingSetConfWriter     )

DECLARE_FACTORY_ENTRIES( CaloRingerAlgs ) {
  DECLARE_ALGORITHM(     CaloRingerAlgorithm     )
  DECLARE_ALGORITHM(    xAODRingSetConfWriter    )
 }

