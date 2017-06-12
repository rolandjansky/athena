// $Id: xAODCreatorAlgs_entries.cxx 298282 2013-12-03 13:05:16Z emoyse $

// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#ifndef XAOD_ANALYSIS

#include "../TrackCollectionCnvTool.h"
#include "../RecTrackParticleContainerCnvTool.h"
#include "../TrackParticleCnvAlg.h"
#include "../VertexCnvAlg.h"

DECLARE_NAMESPACE_TOOL_FACTORY( xAODMaker, RecTrackParticleContainerCnvTool )
DECLARE_NAMESPACE_TOOL_FACTORY( xAODMaker, TrackCollectionCnvTool )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODMaker, TrackParticleCnvAlg )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODMaker, VertexCnvAlg )

#endif //XAOD_ANALYSIS

#include "../TrackParticleCompressorTool.h"
DECLARE_NAMESPACE_TOOL_FACTORY( xAODMaker, TrackParticleCompressorTool )


DECLARE_FACTORY_ENTRIES( xAODCreatorAlgs ) {
   DECLARE_NAMESPACE_TOOL( xAODMaker, TrackParticleCompressorTool )
#ifndef XAOD_ANALYSIS
   DECLARE_NAMESPACE_TOOL( xAODMaker, RecTrackParticleContainerCnvTool )
   DECLARE_NAMESPACE_TOOL( xAODMaker, TrackCollectionCnvTool )
   DECLARE_NAMESPACE_ALGORITHM( xAODMaker, TrackParticleCnvAlg )
   DECLARE_NAMESPACE_ALGORITHM( xAODMaker, VertexCnvAlg )
#endif

}
