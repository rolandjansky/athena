
#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../BeamSpotCondAlg.h"

DECLARE_ALGORITHM_FACTORY( BeamSpotCondAlg )

DECLARE_FACTORY_ENTRIES( BeamSpotConditions ) 
{
  DECLARE_ALGORITHM( BeamSpotCondAlg );
}
