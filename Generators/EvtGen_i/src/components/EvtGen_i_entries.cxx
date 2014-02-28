//====================================================================
//  EVTDECAY_entries.cxx
//--------------------------------------------------------------------
//
//  Package    : Generators/EvtDecay
//
//  Description: Declaration of the components (factory entries)
//               specified by this component library.
//
//====================================================================

#include "EvtGen_i/EvtInclusiveDecay.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( EvtInclusiveDecay )

DECLARE_FACTORY_ENTRIES( EvtGen_i ) {
    DECLARE_ALGORITHM( EvtDecay )
    DECLARE_ALGORITHM( EvtInclusiveDecay )
}
