#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../ReducePileUpEventInfoAlg.h"
#include "../ThinIParticlesAlg.h"
#include "../ThinIParticlesTool.h"
#include "../ThinGeantTruthAlg.h"
#include "../ThinNegativeEnergyCaloClustersAlg.h"
#include "../ThinNegativeEnergyNeutralPFOsAlg.h"
#include "../ThinInDetForwardTrackParticlesAlg.h"
// AthAnalysisBase doesn't know about calo cells (geometry would be needed)
#ifndef XAOD_ANALYSIS
#include "../ThinCaloCellsAlg.h"
#include "../ThinCaloCellsTool.h"
#include "../ThinTrkTrackAlg.h"
#endif
#include "../ThinCaloClustersAlg.h"
#include "../ThinCaloClustersTool.h"
#include "../ThinTrackParticlesAlg.h"
#include "../ThinTrackParticlesTool.h"
#include "../ThinAssociatedObjectsTool.h"
#include "../EleLinkThinningTool.h"
#include "../DeltaRThinningTool.h"

DECLARE_ALGORITHM_FACTORY( ThinInDetForwardTrackParticlesAlg )
DECLARE_ALGORITHM_FACTORY( ThinGeantTruthAlg )
DECLARE_ALGORITHM_FACTORY( ThinNegativeEnergyCaloClustersAlg )
DECLARE_ALGORITHM_FACTORY( ThinNegativeEnergyNeutralPFOsAlg )
DECLARE_ALGORITHM_FACTORY( ReducePileUpEventInfoAlg )
DECLARE_ALGORITHM_FACTORY( ThinIParticlesAlg )
DECLARE_TOOL_FACTORY( ThinIParticlesTool )
// AthAnalysisBase doesn't know about calo cells (geometry would be needed)
#ifndef XAOD_ANALYSIS
DECLARE_ALGORITHM_FACTORY( ThinCaloCellsAlg )
DECLARE_TOOL_FACTORY( ThinCaloCellsTool )
DECLARE_ALGORITHM_FACTORY( ThinTrkTrackAlg )
#endif
DECLARE_ALGORITHM_FACTORY( ThinCaloClustersAlg )
DECLARE_TOOL_FACTORY( ThinCaloClustersTool )
DECLARE_ALGORITHM_FACTORY( ThinTrackParticlesAlg )
DECLARE_TOOL_FACTORY( ThinTrackParticlesTool )
DECLARE_TOOL_FACTORY( ThinAssociatedObjectsTool )
DECLARE_TOOL_FACTORY( EleLinkThinningTool )
DECLARE_TOOL_FACTORY( DeltaRThinningTool )

DECLARE_FACTORY_ENTRIES( ThinningUtils )
{
  DECLARE_ALGORITHM( ThinInDetForwardTrackParticlesAlg );
  DECLARE_ALGORITHM( ThinGeantTruthAlg );
  DECLARE_ALGORITHM( ThinNegativeEnergyCaloClustersAlg);
  DECLARE_ALGORITHM( ThinNegativeEnergyNeutralPFOsAlg);
  DECLARE_ALGORITHM( ReducePileUpEventInfoAlg );
  DECLARE_ALGORITHM( ThinIParticlesAlg );
  DECLARE_TOOL( ThinIParticlesTool );
// AthAnalysisBase doesn't know about calo cells (geometry would be needed)
#ifndef XAOD_ANALYSIS
  DECLARE_ALGORITHM( ThinCaloCellsAlg );
  DECLARE_TOOL( ThinCaloCellsTool );
  DECLARE_ALGORITHM ( ThinTrkTrackAlg );
#endif
  DECLARE_ALGORITHM( ThinCaloClustersAlg );
  DECLARE_TOOL( ThinCaloClustersTool );
  DECLARE_ALGORITHM( ThinTrackParticlesAlg );
  DECLARE_TOOL( ThinTrackParticlesTool );
  DECLARE_TOOL( ThinAssociatedObjectsTool );
  DECLARE_TOOL( EleLinkThinningTool );
  DECLARE_TOOL( DeltaRThinningTool );
}
