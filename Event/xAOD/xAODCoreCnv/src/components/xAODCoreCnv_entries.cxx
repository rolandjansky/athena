// $Id: xAODCoreCnv_entries.cxx 583869 2014-02-18 11:31:09Z krasznaa $

// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "../AuxStoreWrapper.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODMaker, AuxStoreWrapper )

DECLARE_FACTORY_ENTRIES( xAODCreatorAlgs ) {

   DECLARE_NAMESPACE_ALGORITHM( xAODMaker, AuxStoreWrapper )

}
