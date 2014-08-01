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

#include "Pyquench_i/Pyquench.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( Pyquench )

DECLARE_FACTORY_ENTRIES( Pyquench_i ) {
    DECLARE_ALGORITHM( Pyquench )
}
