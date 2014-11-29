// $Id: xAODTauJetCnv_entries.cxx 632174 2014-11-29 15:23:36Z krasznaa $

// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "../TauJetCnvAlg.h"
#include "../TauJetCnvTool.h"
#include "../TauJetReaderAlg.h"


DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODMaker, TauJetCnvAlg ) 
DECLARE_NAMESPACE_TOOL_FACTORY( xAODMaker, TauJetCnvTool ) 
DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODMaker, TauJetReaderAlg )

DECLARE_FACTORY_ENTRIES( xAODTauJetCnv ) {

  DECLARE_NAMESPACE_ALGORITHM( xAODMaker, TauJetCnvAlg )
  DECLARE_NAMESPACE_TOOL( xAODMaker, TauJetCnvTool )
  DECLARE_NAMESPACE_ALGORITHM( xAODMaker, TauJetReaderAlg )

}
