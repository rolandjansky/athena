#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrigRoiConversion/RoiWriter.h"


DECLARE_ALGORITHM_FACTORY( RoiWriter )


DECLARE_FACTORY_ENTRIES(TrigEoiConversion) {
    DECLARE_ALGORITHM( RoiWriter )
}
