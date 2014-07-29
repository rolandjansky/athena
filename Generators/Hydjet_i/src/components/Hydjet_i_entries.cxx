//--------------------------------------------------------------------
//
//  Package    : Generators/Hydjet_i
//
//  Description: Declaration of the components (factory entries)
//               specified by this component library.
//
//====================================================================

#include "Hydjet_i/Hydjet_i.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( Hydjet )

DECLARE_FACTORY_ENTRIES( Hydjet_i ) {
    DECLARE_ALGORITHM( Hydjet )
}
