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

#include "Herwigpp_i/Herwigpp.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY(Herwigpp)

DECLARE_FACTORY_ENTRIES(Herwigpp_i) {
  DECLARE_ALGORITHM( Herwigpp )
}
