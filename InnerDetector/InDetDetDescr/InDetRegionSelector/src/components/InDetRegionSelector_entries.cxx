#include "InDetRegionSelector/InDetRegionSelectorLUT.h"
#include "InDetRegionSelector/SiRegionSelectorTable.h"
#include "InDetRegionSelector/FTK_RegionSelectorTable.h"
#include "InDetRegionSelector/TRT_RegionSelectorTable.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY(InDetDD, InDetRegionSelectorLUT )
DECLARE_TOOL_FACTORY( SiRegionSelectorTable )
DECLARE_TOOL_FACTORY( TRT_RegionSelectorTable )
DECLARE_TOOL_FACTORY( FTK_RegionSelectorTable )

DECLARE_FACTORY_ENTRIES(InDetRegionSelector) {
    DECLARE_NAMESPACE_ALGORITHM( InDetDD, InDetRegionSelectorLUT )
    DECLARE_TOOL( SiRegionSelectorTable )
    DECLARE_TOOL( TRT_RegionSelectorTable )
    DECLARE_TOOL( FTK_RegionSelectorTable )
}
