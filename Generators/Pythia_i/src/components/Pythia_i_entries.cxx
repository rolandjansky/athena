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

#include "Pythia_i/Pythia.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( Pythia )

DECLARE_FACTORY_ENTRIES( Pythia_i ) {
    DECLARE_ALGORITHM( Pythia )
}
