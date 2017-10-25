#include "../BTaggingCnvAlg.h"
#include "../BTaggingReader.h"
#include "../TrigBjetCnvAlg.h"
#include "../TrigBjetCnvTool.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODMaker, BTaggingCnvAlg )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODReader, BTaggingReader )
	
DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODMaker, TrigBjetCnvAlg )
DECLARE_NAMESPACE_TOOL_FACTORY( xAODMaker, TrigBjetCnvTool )

