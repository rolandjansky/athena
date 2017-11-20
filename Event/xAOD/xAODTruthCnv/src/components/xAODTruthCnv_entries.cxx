#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "../xAODTruthCnvAlg.h"
#include "../xAODTruthMetaDataTool.h"
#include "../xAODTruthReader.h"
#include "../HepMCTruthReader.h"
#include "xAODTruthCnv/TruthMetaDataTool.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODMaker, xAODTruthCnvAlg )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODReader, xAODTruthReader )
DECLARE_ALGORITHM_FACTORY( HepMCTruthReader )
DECLARE_NAMESPACE_TOOL_FACTORY( xAODMaker, TruthMetaDataTool )
DECLARE_NAMESPACE_TOOL_FACTORY( xAODMaker, xAODTruthMetaDataTool )

DECLARE_FACTORY_ENTRIES( xAODTruthCnv ) {
   DECLARE_NAMESPACE_ALGORITHM( xAODMaker, xAODTruthCnvAlg )
   DECLARE_NAMESPACE_ALGORITHM( xAODReader, xAODTruthReader )
   DECLARE_ALGORITHM( HepMCTruthReader )
   DECLARE_NAMESPACE_TOOL( xAODMaker, TruthMetaDataTool )
   DECLARE_NAMESPACE_TOOL( xAODMaker, xAODTruthMetaDataTool )
}

