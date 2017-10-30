#include "../ReducePileUpEventInfoAlg.h"
#include "../ThinIParticlesAlg.h"
#include "../ThinIParticlesTool.h"
#include "../ThinCaloCellsAlg.h"
#include "../ThinGeantTruthAlg.h"
#include "../ThinNegativeEnergyCaloClustersAlg.h"
#include "../ThinNegativeEnergyNeutralPFOsAlg.h"
// AthAnalysisBase doesn't know about calo cells (geometry would be needed)
#ifndef XAOD_ANALYSIS
#include "../ThinCaloCellsTool.h"
#include "../ThinTrkTrackAlg.h"
#endif
#include "../ThinCaloClustersAlg.h"
#include "../ThinCaloClustersTool.h"
#include "../ThinTrackParticlesAlg.h"
#include "../ThinTrackParticlesTool.h"

DECLARE_ALGORITHM_FACTORY( ThinGeantTruthAlg )
DECLARE_ALGORITHM_FACTORY( ThinNegativeEnergyCaloClustersAlg )
DECLARE_ALGORITHM_FACTORY( ThinNegativeEnergyNeutralPFOsAlg )
DECLARE_ALGORITHM_FACTORY( ReducePileUpEventInfoAlg )
DECLARE_ALGORITHM_FACTORY( ThinIParticlesAlg )
DECLARE_TOOL_FACTORY( ThinIParticlesTool )
DECLARE_ALGORITHM_FACTORY( ThinCaloCellsAlg )
// AthAnalysisBase doesn't know about calo cells (geometry would be needed)
#ifndef XAOD_ANALYSIS
DECLARE_TOOL_FACTORY( ThinCaloCellsTool )
DECLARE_ALGORITHM_FACTORY( ThinTrkTrackAlg )
#endif
DECLARE_ALGORITHM_FACTORY( ThinCaloClustersAlg )
DECLARE_TOOL_FACTORY( ThinCaloClustersTool )
DECLARE_ALGORITHM_FACTORY( ThinTrackParticlesAlg )
DECLARE_TOOL_FACTORY( ThinTrackParticlesTool )

