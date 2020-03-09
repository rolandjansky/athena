#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../BTagVertexAugmenter.h"
#include "../BTagJetAugmenterAlg.h"

DECLARE_ALGORITHM_FACTORY(BTagVertexAugmenter)
DECLARE_ALGORITHM_FACTORY(BTagJetAugmenterAlg)

DECLARE_FACTORY_ENTRIES( DerivationFrameworkFlavourTag )
{
  DECLARE_ALGORITHM( BTagVertexAugmenter );
  DECLARE_ALGORITHM( BTagJetAugmenterAlg );
  
}
