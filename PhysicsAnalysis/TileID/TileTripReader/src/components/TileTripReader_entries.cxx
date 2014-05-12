#include "GaudiKernel/DeclareFactoryEntries.h"

#include "TileTripReader/AthTileTripReader.h"

DECLARE_TOOL_FACTORY(AthTileTripReader)

DECLARE_FACTORY_ENTRIES(TileTripReader) {
    DECLARE_ALGTOOL(AthTileTripReader)
}
