// $Id: xAODCoreCnv_entries.cxx 803546 2017-04-25 09:44:26Z krasznaa $

// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "../AuxStoreWrapper.h"
#include "../ElementLinkResetAlg.h"
#include "../DynVarFixerAlg.h"
#include "../DynVarToDecorationAlg.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODMaker, AuxStoreWrapper )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODMaker, ElementLinkResetAlg )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODMaker, DynVarFixerAlg )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODMaker, DynVarToDecorationAlg )

DECLARE_FACTORY_ENTRIES( xAODCoreCnv ) {

   DECLARE_NAMESPACE_ALGORITHM( xAODMaker, AuxStoreWrapper )
   DECLARE_NAMESPACE_ALGORITHM( xAODMaker, ElementLinkResetAlg )
   DECLARE_NAMESPACE_ALGORITHM( xAODMaker, DynVarFixerAlg )
   DECLARE_NAMESPACE_ALGORITHM( xAODMaker, DynVarToDecorationAlg )

}
