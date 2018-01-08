#include <GaudiKernel/DeclareFactoryEntries.h>

#include "../ArtTest.h"

DECLARE_COMPONENT( ArtTest )

DECLARE_ALGORITHM_FACTORY (ArtTest)

DECLARE_FACTORY_ENTRIES  ( egammaValidation )
{

  DECLARE_ALGORITHM (ArtTest);

}
