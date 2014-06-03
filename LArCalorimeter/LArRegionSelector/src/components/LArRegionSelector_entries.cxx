#include "LArRegionSelector/LArRegionSelectorTable.h"
#include "LArRegionSelector/LArRegionSelectorCheckOL.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY( LArRegionSelectorTable )
DECLARE_ALGORITHM_FACTORY( LArRegionSelectorCheckOL )

DECLARE_FACTORY_ENTRIES(LArRegionSelector) {
    DECLARE_TOOL( LArRegionSelectorTable )
    DECLARE_ALGORITHM( LArRegionSelectorCheckOL )
}
