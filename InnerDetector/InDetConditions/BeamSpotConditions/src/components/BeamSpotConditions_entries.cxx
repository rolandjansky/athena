
#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../BeamSpotCondAlg.h"

DECLARE_ALGORITHM_FACTORY( BeamSpotCondAlg )

DECLARE_FACTORY_ENTRIES( BeamSpotConditionsComps ) 
{
  DECLARE_ALGORITHM( BeamSpotCondAlg );
}
