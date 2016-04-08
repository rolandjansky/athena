#include "LArClusterRec/LArTBClusterBuilder.h"
#include "LArClusterRec/LArDigitThinnerFromEMClust.h"
#include "LArClusterRec/LArClusterCollisionTimeAlg.h"
#include "GaudiKernel/DeclareFactoryEntries.h"


DECLARE_TOOL_FACTORY( LArTBClusterBuilder )
DECLARE_ALGORITHM_FACTORY( LArDigitThinnerFromEMClust )
DECLARE_ALGORITHM_FACTORY( LArClusterCollisionTimeAlg )

DECLARE_FACTORY_ENTRIES(LArClusterRec) {
    DECLARE_TOOL( LArTBClusterBuilder )
    DECLARE_ALGORITHM( LArDigitThinnerFromEMClust )
    DECLARE_ALGORITHM( LArClusterCollisionTimeAlg )
}
