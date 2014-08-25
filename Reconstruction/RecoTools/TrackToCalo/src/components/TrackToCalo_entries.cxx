//====================================================================

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrackToCalo/ExtrapolateToCaloTool.h"
#include "../ParticleCaloExtensionTool.h"
#include "../ParticleCaloCellAssociationTool.h"
#include "../TrackParticleCaloExtensionAlg.h"
#include "TrackToCalo/ImpactInCaloAlg.h"

using namespace Trk;
DECLARE_TOOL_FACTORY( ExtrapolateToCaloTool )
DECLARE_TOOL_FACTORY( ParticleCaloExtensionTool )
DECLARE_TOOL_FACTORY( ParticleCaloCellAssociationTool )
DECLARE_ALGORITHM_FACTORY( ImpactInCaloAlg )
DECLARE_ALGORITHM_FACTORY( TrackParticleCaloExtensionAlg )

DECLARE_FACTORY_ENTRIES	( TrackToCalo ) {
    DECLARE_TOOL( ExtrapolateToCaloTool );
    DECLARE_TOOL( ParticleCaloExtensionTool );
    DECLARE_TOOL( ParticleCaloCellAssociationTool );
    DECLARE_ALGORITHM( ImpactInCaloAlg );
    DECLARE_ALGORITHM( TrackParticleCaloExtensionAlg );
}


