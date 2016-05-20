// $Id: xAODCoreCnv_entries.cxx 748595 2016-05-20 10:57:20Z krasznaa $

// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "../AuxStoreWrapper.h"
#include "../ElementLinkResetAlg.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODMaker, AuxStoreWrapper )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODMaker, ElementLinkResetAlg )

DECLARE_FACTORY_ENTRIES( xAODCoreCnv ) {

   DECLARE_NAMESPACE_ALGORITHM( xAODMaker, AuxStoreWrapper )
   DECLARE_NAMESPACE_ALGORITHM( xAODMaker, ElementLinkResetAlg )

}
