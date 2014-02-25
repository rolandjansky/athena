#include "LArMuId/LArMuIdAlgorithm.h"
#include "LArMuId/LArDigitThinnerFromIIC.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( LArMuIdAlgorithm )
DECLARE_ALGORITHM_FACTORY( LArDigitThinnerFromIIC )

DECLARE_FACTORY_ENTRIES(LArMuId) {
    DECLARE_ALGORITHM( LArMuIdAlgorithm )
    DECLARE_ALGORITHM( LArDigitThinnerFromIIC )
}
