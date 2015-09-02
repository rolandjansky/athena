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
#include "QGSJet_i/QGSJet.h"

DECLARE_ALGORITHM_FACTORY( QGSJet )

DECLARE_FACTORY_ENTRIES( QGSJet_i ) {
    DECLARE_ALGORITHM( QGSJet )
}
