#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../BTagVertexAugmenter.h"
#include "../BTagTrackAugmenter.h"

DECLARE_ALGORITHM_FACTORY(BTagVertexAugmenter)
DECLARE_ALGORITHM_FACTORY(BTagTrackAugmenter)

DECLARE_FACTORY_ENTRIES( DerivationFrameworkFlavourTag )
{
  DECLARE_ALGORITHM( BTagVertexAugmenter );
  DECLARE_ALGORITHM( BTagTrackAugmenter );
}
