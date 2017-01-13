#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../ReducePileUpEventInfoAlg.h"
#include "../ThinIParticlesAlg.h"
#include "../ThinIParticlesTool.h"
#include "../ThinCaloCellsAlg.h"
#include "../ThinGeantTruthAlg.h"
// AthAnalysisBase doesn't know about calo cells (geometry would be needed)
#ifndef XAOD_ANALYSIS
#include "../ThinCaloCellsTool.h"
#endif
#include "../ThinCaloClustersAlg.h"
#include "../ThinCaloClustersTool.h"
#include "../ThinTrackParticlesAlg.h"
#include "../ThinTrackParticlesTool.h"

DECLARE_ALGORITHM_FACTORY( ThinGeantTruthAlg )
DECLARE_ALGORITHM_FACTORY( ReducePileUpEventInfoAlg )
DECLARE_ALGORITHM_FACTORY( ThinIParticlesAlg )
DECLARE_TOOL_FACTORY( ThinIParticlesTool )
DECLARE_ALGORITHM_FACTORY( ThinCaloCellsAlg )
// AthAnalysisBase doesn't know about calo cells (geometry would be needed)
#ifndef XAOD_ANALYSIS
DECLARE_TOOL_FACTORY( ThinCaloCellsTool )
#endif
DECLARE_ALGORITHM_FACTORY( ThinCaloClustersAlg )
DECLARE_TOOL_FACTORY( ThinCaloClustersTool )
DECLARE_ALGORITHM_FACTORY( ThinTrackParticlesAlg )
DECLARE_TOOL_FACTORY( ThinTrackParticlesTool )

DECLARE_FACTORY_ENTRIES( ThinningUtils )
{
  DECLARE_ALGORITHM( ThinGeantTruthAlg );
  DECLARE_ALGORITHM( ReducePileUpEventInfoAlg );
  DECLARE_ALGORITHM( ThinIParticlesAlg );
  DECLARE_TOOL( ThinIParticlesTool );
  DECLARE_ALGORITHM( ThinCaloCellsAlg );
// AthAnalysisBase doesn't know about calo cells (geometry would be needed)
#ifndef XAOD_ANALYSIS
  DECLARE_TOOL( ThinCaloCellsTool );
#endif
  DECLARE_ALGORITHM( ThinCaloClustersAlg );
  DECLARE_TOOL( ThinCaloClustersTool );
  DECLARE_ALGORITHM( ThinTrackParticlesAlg );
  DECLARE_TOOL( ThinTrackParticlesTool );
}
