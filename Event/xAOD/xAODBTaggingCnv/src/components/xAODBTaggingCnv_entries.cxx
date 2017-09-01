// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "../BTaggingCnvAlg.h"
#include "../BTaggingReader.h"
#include "../TrigBjetCnvAlg.h"
#include "../TrigBjetCnvTool.h"
#include "../BTaggingAuxFixerAlg.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODMaker, BTaggingCnvAlg )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODReader, BTaggingReader )
	
DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODMaker, TrigBjetCnvAlg )
DECLARE_NAMESPACE_TOOL_FACTORY( xAODMaker, TrigBjetCnvTool )

DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODMaker, BTaggingAuxFixerAlg )

DECLARE_FACTORY_ENTRIES( xAODBTaggingCnv ) {
  DECLARE_NAMESPACE_ALGORITHM( xAODMaker, BTaggingCnvAlg )
  DECLARE_NAMESPACE_ALGORITHM( xAODReader, BTaggingReader )

  DECLARE_NAMESPACE_ALGORITHM( xAODMaker, TrigBJetCnvAlg )
  DECLARE_NAMESPACE_TOOL( xAODMaker, TrigBJetCnvTool )

  DECLARE_NAMESPACE_ALGORITHM( xAODMaker, BTaggingAuxFixerAlg )
}
