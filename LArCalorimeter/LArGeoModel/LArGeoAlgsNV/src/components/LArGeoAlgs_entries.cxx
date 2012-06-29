#include "LArGeoAlgsNV/LArDetectorToolNV.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY(LArDetectorToolNV)

DECLARE_FACTORY_ENTRIES(LArGeoAlgsNV) {
    DECLARE_ALGTOOL  ( LArDetectorToolNV )
}
