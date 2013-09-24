//====================================================================
//  FastCaloSimHit_entries.cxx
//--------------------------------------------------------------------
//
//  Package    : Simulation/FastShower/FastCaloSimHit
//
//  Description: Implementation of <Package>_load routine.
//               This routine is needed for forcing the linker
//               to load all the components of the library. 
//
//====================================================================

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "FastCaloSimHit/FastHitConv.h"
#include "FastCaloSimHit/FastHitConvertTool.h"

DECLARE_ALGORITHM_FACTORY( FastHitConv )
DECLARE_TOOL_FACTORY(FastHitConvertTool)

DECLARE_FACTORY_ENTRIES(FastCaloSimHit) {
    DECLARE_ALGORITHM( FastHitConv );
    DECLARE_TOOL(FastHitConvertTool);
}

