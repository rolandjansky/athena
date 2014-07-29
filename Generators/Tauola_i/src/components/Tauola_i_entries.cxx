//====================================================================
//  GeneratorModules_entries.cxx
//--------------------------------------------------------------------
//
//  Package    : Generators/GeneratorModules
//
//  Description: Declaration of the components (factory entries)
//               specified by this component library.
//
//====================================================================

#include "Tauola_i/Tauola.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( Tauola )

DECLARE_FACTORY_ENTRIES( Tauola_i ) {
    DECLARE_ALGORITHM( Tauola )
}
