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

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "Epos_i/Epos.h"

DECLARE_ALGORITHM_FACTORY( Epos )

DECLARE_FACTORY_ENTRIES( Epos_i ) {
    DECLARE_ALGORITHM( Epos )
}
