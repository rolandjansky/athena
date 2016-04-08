#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrkTruthAlgs/TrackTruthSelector.h"
#include "TrkTruthAlgs/TrackTruthSimilaritySelector.h"
#include "../TrackParticleTruthAlg.h"

DECLARE_ALGORITHM_FACTORY( TrackTruthSelector )
DECLARE_ALGORITHM_FACTORY( TrackTruthSimilaritySelector )
DECLARE_ALGORITHM_FACTORY( TrackParticleTruthAlg )

DECLARE_FACTORY_ENTRIES( TrkTruthAlgs ) 
{
  DECLARE_ALGORITHM( TrackTruthSelector )
  DECLARE_ALGORITHM( TrackTruthSimilaritySelector )
  DECLARE_ALGORITHM( TrackParticleTruthAlg )
}
