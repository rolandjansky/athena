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
#include "../WriteDataReentrant.h"
#include "../ReadDataReentrant.h"
#include "../HandleTestTool1.h"
#include "../HandleTestTool2.h"
#include "../HandleTestAlg.h"

DECLARE_COMPONENT( ReadData )
DECLARE_COMPONENT( ReadPileUpData )
DECLARE_COMPONENT( WriteData )
DECLARE_COMPONENT( WriteDataReentrant )
DECLARE_COMPONENT( ReadDataReentrant )
DECLARE_COMPONENT( AthEx::HandleTestTool1 )
DECLARE_COMPONENT( AthEx::HandleTestTool2 )
DECLARE_COMPONENT( AthEx::HandleTestAlg )

