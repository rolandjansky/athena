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

