#include "LArG4Validation/SingleTrackValidation.h"
#include "LArG4Validation/AODReader.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( SingleTrackValidation )
DECLARE_ALGORITHM_FACTORY( AODReader )

DECLARE_FACTORY_ENTRIES(LArG4Validation) {
    DECLARE_ALGORITHM( SingleTrackValidation )
    DECLARE_ALGORITHM( AODReader )
}



