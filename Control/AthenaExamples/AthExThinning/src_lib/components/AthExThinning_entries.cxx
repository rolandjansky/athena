#include "AthExThinning/CreateData.h"
#include "AthExThinning/WriteThinnedData.h"
#include "AthExThinning/ReadThinnedData.h"
#include "AthExThinning/SlimmerAlg.h"

#include "GaudiKernel/DeclareFactoryEntries.h"
  
DECLARE_NAMESPACE_ALGORITHM_FACTORY( AthExThinning, CreateData )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( AthExThinning, WriteThinnedData )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( AthExThinning, ReadThinnedData )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( AthExThinning, SlimmerAlg )

DECLARE_FACTORY_ENTRIES( AthExThinningAlgs ) {
  DECLARE_NAMESPACE_ALGORITHM( AthExThinning, CreateData )
  DECLARE_NAMESPACE_ALGORITHM( AthExThinning, WriteThinnedData )
  DECLARE_NAMESPACE_ALGORITHM( AthExThinning, ReadThinnedData )
  DECLARE_NAMESPACE_ALGORITHM( AthExThinning, SlimmerAlg )
}
