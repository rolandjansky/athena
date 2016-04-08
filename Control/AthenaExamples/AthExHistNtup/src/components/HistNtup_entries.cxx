//====================================================================
//  HistNtup_entries.cxx
//--------------------------------------------------------------------
//
//  Package    : HistNtup
//
//  Description: Implementation of <Package>_load routine.
//               This routine is needed for forcing the linker
//               to load all the components of the library. 
//
//====================================================================

#include "../Hist.h"
#include "../Ntup.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

using namespace AthEx;

DECLARE_ALGORITHM_FACTORY( Hist )
DECLARE_ALGORITHM_FACTORY( Ntup )

DECLARE_FACTORY_ENTRIES(AthExHistNtup) {
    DECLARE_ALGORITHM( Hist )
    DECLARE_ALGORITHM( Ntup )
}
