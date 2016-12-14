// $Id: xAODCreatorAlgs_entries.cxx 298282 2013-12-03 13:05:16Z emoyse $

// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "../TrackCollectionCnvTool.h"
#include "../RecTrackParticleContainerCnvTool.h"
#include "../TrackParticleCnvAlg.h"
#include "../VertexCnvAlg.h"
#include "../TrackParticleCompressorTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY( xAODMaker, RecTrackParticleContainerCnvTool )
DECLARE_NAMESPACE_TOOL_FACTORY( xAODMaker, TrackCollectionCnvTool )
DECLARE_NAMESPACE_TOOL_FACTORY( xAODMaker, TrackParticleCompressorTool )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODMaker, TrackParticleCnvAlg )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODMaker, VertexCnvAlg )

DECLARE_FACTORY_ENTRIES( xAODCreatorAlgs ) {
   DECLARE_NAMESPACE_TOOL( xAODMaker, RecTrackParticleContainerCnvTool )
   DECLARE_NAMESPACE_TOOL( xAODMaker, TrackCollectionCnvTool )
   DECLARE_NAMESPACE_TOOL( xAODMaker, TrackParticleCompressorTool )
   DECLARE_NAMESPACE_ALGORITHM( xAODMaker, TrackParticleCnvAlg )
   DECLARE_NAMESPACE_ALGORITHM( xAODMaker, VertexCnvAlg )
}
