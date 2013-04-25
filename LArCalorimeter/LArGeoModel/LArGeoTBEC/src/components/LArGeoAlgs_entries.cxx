#include "LArGeoTBEC/LArDetectorToolTBEC.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY(LArDetectorToolTBEC)

DECLARE_FACTORY_ENTRIES(LArGeoTBEC) {
    DECLARE_ALGTOOL  ( LArDetectorToolTBEC )
}
