#include "MuonRegionSelector/RPC_RegionSelectorTable.h"
#include "MuonRegionSelector/TGC_RegionSelectorTable.h"
#include "MuonRegionSelector/MDT_RegionSelectorTable.h"
#include "MuonRegionSelector/CSC_RegionSelectorTable.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

// DECLARE_NAMESPACE_ALGORITHM_FACTORY(InDetDD, InDetRegionSelectorLUT )
DECLARE_TOOL_FACTORY( RPC_RegionSelectorTable )
DECLARE_TOOL_FACTORY( TGC_RegionSelectorTable )
DECLARE_TOOL_FACTORY( MDT_RegionSelectorTable )
DECLARE_TOOL_FACTORY( CSC_RegionSelectorTable )

DECLARE_FACTORY_ENTRIES(MuonRegionSelector) {
   //    DECLARE_NAMESPACE_ALGORITHM( InDetDD, InDetRegionSelectorLUT )
   DECLARE_TOOL( RPC_RegionSelectorTable )
   DECLARE_TOOL( TGC_RegionSelectorTable )
   DECLARE_TOOL( MDT_RegionSelectorTable )
   DECLARE_TOOL( CSC_RegionSelectorTable )
}
