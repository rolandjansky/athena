#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../TruthTrackCreation.h"

using namespace Trk;
 
DECLARE_ALGORITHM_FACTORY(TruthTrackCreation)

    
DECLARE_FACTORY_ENTRIES(TrkTruthTrackAlgs)
{
  DECLARE_ALGORITHM(TruthTrackCreation)      
}
