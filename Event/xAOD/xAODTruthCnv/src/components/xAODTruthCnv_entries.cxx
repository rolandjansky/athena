#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "../xAODTruthCnvAlg.h"
#include "../xAODTruthReader.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODMaker, xAODTruthCnvAlg )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODReader, xAODTruthReader )


DECLARE_FACTORY_ENTRIES( xAODCreatorAlgs ) {
   DECLARE_NAMESPACE_ALGORITHM( xAODMaker, xAODTruthCnvAlg )
   DECLARE_NAMESPACE_ALGORITHM( xAODReader, xAODTruthReader )
}
