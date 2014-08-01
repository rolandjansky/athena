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

#include "PythiaRhad_i/PythiaRhad.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( PythiaRhad )

DECLARE_FACTORY_ENTRIES( PythiaRhad_i ) {
    DECLARE_ALGORITHM( PythiaRhad )
}
