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

DECLARE_COMPONENT( ThinGeantTruthAlg )
DECLARE_COMPONENT( ThinNegativeEnergyCaloClustersAlg )
DECLARE_COMPONENT( ThinNegativeEnergyNeutralPFOsAlg )
DECLARE_COMPONENT( ReducePileUpEventInfoAlg )
DECLARE_COMPONENT( ThinIParticlesAlg )
DECLARE_COMPONENT( ThinIParticlesTool )
DECLARE_COMPONENT( ThinCaloCellsAlg )
// AthAnalysisBase doesn't know about calo cells (geometry would be needed)
#ifndef XAOD_ANALYSIS
DECLARE_COMPONENT( ThinCaloCellsTool )
DECLARE_COMPONENT( ThinTrkTrackAlg )
#endif
DECLARE_COMPONENT( ThinCaloClustersAlg )
DECLARE_COMPONENT( ThinCaloClustersTool )
DECLARE_COMPONENT( ThinTrackParticlesAlg )
DECLARE_COMPONENT( ThinTrackParticlesTool )

