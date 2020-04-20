#include "MuonRegionSelector/RPC_RegionSelectorTable.h"
#include "MuonRegionSelector/TGC_RegionSelectorTable.h"
#include "MuonRegionSelector/MDT_RegionSelectorTable.h"
#include "MuonRegionSelector/CSC_RegionSelectorTable.h"
#include "MuonRegionSelector/MM_RegionSelectorTable.h"
#include "MuonRegionSelector/sTGC_RegionSelectorTable.h"

// DECLARE_COMPONENT( InDetDD::InDetRegionSelectorLUT )
DECLARE_COMPONENT( RPC_RegionSelectorTable )
DECLARE_COMPONENT( TGC_RegionSelectorTable )
DECLARE_COMPONENT( MDT_RegionSelectorTable )
DECLARE_COMPONENT( CSC_RegionSelectorTable )
DECLARE_COMPONENT( MM_RegionSelectorTable )
DECLARE_COMPONENT( sTGC_RegionSelectorTable )

#include  "../MDT_RegSelCondAlg.h"
#include  "../RPC_RegSelCondAlg.h"
#include  "../TGC_RegSelCondAlg.h"
#include  "../CSC_RegSelCondAlg.h"
#include   "../MM_RegSelCondAlg.h"
#include "../sTGC_RegSelCondAlg.h"

DECLARE_COMPONENT(  MDT_RegSelCondAlg )
DECLARE_COMPONENT(  RPC_RegSelCondAlg )
DECLARE_COMPONENT(  TGC_RegSelCondAlg )
DECLARE_COMPONENT(  CSC_RegSelCondAlg )
DECLARE_COMPONENT(   MM_RegSelCondAlg )
DECLARE_COMPONENT( sTGC_RegSelCondAlg )

