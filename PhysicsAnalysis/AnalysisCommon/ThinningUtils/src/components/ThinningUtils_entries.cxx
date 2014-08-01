#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../ReducePileUpEventInfoAlg.h"
#include "../ThinIParticlesAlg.h"
#include "../ThinIParticlesTool.h"
#include "../ThinCaloCellsAlg.h"
#include "../ThinCaloCellsTool.h"
#include "../ThinCaloClustersAlg.h"
#include "../ThinCaloClustersTool.h"
#include "../ThinTrackParticlesAlg.h"
#include "../ThinTrackParticlesTool.h"

DECLARE_ALGORITHM_FACTORY( ReducePileUpEventInfoAlg )
DECLARE_ALGORITHM_FACTORY( ThinIParticlesAlg )
DECLARE_TOOL_FACTORY( ThinIParticlesTool )
DECLARE_ALGORITHM_FACTORY( ThinCaloCellsAlg )
DECLARE_TOOL_FACTORY( ThinCaloCellsTool )
DECLARE_ALGORITHM_FACTORY( ThinCaloClustersAlg )
DECLARE_TOOL_FACTORY( ThinCaloClustersTool )
DECLARE_ALGORITHM_FACTORY( ThinTrackParticlesAlg )
DECLARE_TOOL_FACTORY( ThinTrackParticlesTool )

DECLARE_FACTORY_ENTRIES( ThinningUtils )
{
  DECLARE_ALGORITHM( ReducePileUpEventInfoAlg );
  DECLARE_ALGORITHM( ThinIParticlesAlg );
  DECLARE_TOOL( ThinIParticlesTool );
  DECLARE_ALGORITHM( ThinCaloCellsAlg );
  DECLARE_TOOL( ThinCaloCellsTool );
  DECLARE_ALGORITHM( ThinCaloClustersAlg );
  DECLARE_TOOL( ThinCaloClustersTool );
  DECLARE_ALGORITHM( ThinTrackParticlesAlg );
  DECLARE_TOOL( ThinTrackParticlesTool );
}
