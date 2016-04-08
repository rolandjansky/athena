#include "GaudiKernel/DeclareFactoryEntries.h"

#include "LArAlignmentAlgs/LArAlignDbAlg.h"

DECLARE_ALGORITHM_FACTORY(LArAlignDbAlg)

DECLARE_FACTORY_ENTRIES(LArAlignmentAlgs) {
  DECLARE_ALGORITHM( LArAlignDbAlg );
}
