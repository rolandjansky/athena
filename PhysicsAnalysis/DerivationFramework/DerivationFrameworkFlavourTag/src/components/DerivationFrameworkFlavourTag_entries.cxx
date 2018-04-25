#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../BTagVertexAugmenter.h"
#include "../BTagTrackAugmenter.h"
#include "../HbbTrackThinner.h"
#include "../HbbTaggingAlgorithm.h"
using namespace DerivationFramework;

DECLARE_ALGORITHM_FACTORY(BTagVertexAugmenter)
DECLARE_ALGORITHM_FACTORY(BTagTrackAugmenter)
DECLARE_ALGORITHM_FACTORY(HbbTaggingAlgorithm)
DECLARE_TOOL_FACTORY(HbbTrackThinner)

DECLARE_FACTORY_ENTRIES( DerivationFrameworkFlavourTag )
{
  DECLARE_ALGORITHM( BTagVertexAugmenter );
  DECLARE_ALGORITHM( BTagTrackAugmenter );
  DECLARE_ALGORITHM( HbbTaggingAlgorithm );
  DECLARE_TOOL( HbbTrackThinner );
}
