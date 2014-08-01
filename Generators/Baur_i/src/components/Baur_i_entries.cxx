//--------------------------------------------------------------------
//
//  Package    : Generators/Hijing_i
//
//  Description: Declaration of the components (factory entries)
//               specified by this component library.
//
//====================================================================

#include "Baur_i/Baur.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( Baur )

DECLARE_FACTORY_ENTRIES( Baur_i ) {
    DECLARE_ALGORITHM( Baur )
}
