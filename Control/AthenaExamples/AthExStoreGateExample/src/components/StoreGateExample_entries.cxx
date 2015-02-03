//====================================================================
//  StoreGateExample_entries.cxx
//--------------------------------------------------------------------
//
//  Package    : StoreGate/example
//
//  Description: Implementation of <Package>_load routine.
//               This routine is needed for forcing the linker
//               to load all the components of the library. 
//
//====================================================================

#include "../ReadData.h"
#include "../ReadPileUpData.h"
#include "../WriteData.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( ReadData )
DECLARE_ALGORITHM_FACTORY( ReadPileUpData )
DECLARE_ALGORITHM_FACTORY( WriteData )

DECLARE_FACTORY_ENTRIES(AthExStoreGateExample) {
    DECLARE_ALGORITHM( ReadData )
    DECLARE_ALGORITHM( ReadPileUpData )
    DECLARE_ALGORITHM( WriteData )
}
