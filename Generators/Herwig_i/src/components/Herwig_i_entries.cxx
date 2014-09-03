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

#include "Herwig_i/Herwig.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( Herwig )

DECLARE_FACTORY_ENTRIES(Herwig_i) {
    DECLARE_ALGORITHM( Herwig )
}
