// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):

#include "../JetCnvAlg.h"
#include "../JetCnvTool.h"
#include "../JetGlobalEventSetup.h"



DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODMaker, JetCnvAlg ) 
DECLARE_NAMESPACE_TOOL_FACTORY( xAODMaker, JetCnvTool ) 

DECLARE_ALGORITHM_FACTORY(  JetGlobalEventSetup ) 

DECLARE_FACTORY_ENTRIES( xAODJetCnv ) {


  DECLARE_NAMESPACE_ALGORITHM( xAODMaker, JetCnvAlg ) ;
  DECLARE_NAMESPACE_TOOL( xAODMaker, JetCnvTool ) ;
  DECLARE_ALGORITHM( JetGlobalEventSetup ) ;


}
