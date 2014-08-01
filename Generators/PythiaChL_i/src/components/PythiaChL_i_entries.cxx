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

#include "PythiaChL_i/PythiaChL.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( PythiaChL )

DECLARE_FACTORY_ENTRIES( PythiaChL_i ) {
    DECLARE_ALGORITHM( PythiaChL )
}
