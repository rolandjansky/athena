#include "../ReducePileUpEventInfoAlg.h"
#include "../ThinIParticlesAlg.h"
#include "../ThinIParticlesTool.h"
#include "../ThinGeantTruthAlg.h"
#include "../ThinNegativeEnergyCaloClustersAlg.h"
#include "../ThinNegativeEnergyNeutralPFOsAlg.h"
#include "../ThinInDetForwardTrackParticlesAlg.h"
// AthAnalysisBase doesn't know about Trk::Tracks
#ifndef XAOD_ANALYSIS
#include "../ThinTrkTrackAlg.h"
#endif
#include "../ThinTRTStandaloneTrackAlg.h"

DECLARE_COMPONENT( ThinInDetForwardTrackParticlesAlg )
DECLARE_COMPONENT( ThinGeantTruthAlg )
DECLARE_COMPONENT( ThinNegativeEnergyCaloClustersAlg )
DECLARE_COMPONENT( ThinNegativeEnergyNeutralPFOsAlg )
DECLARE_COMPONENT( ReducePileUpEventInfoAlg )
DECLARE_COMPONENT( ThinIParticlesAlg )
DECLARE_COMPONENT( ThinIParticlesTool )
// AthAnalysisBase doesn't know about Trk::Tracks
#ifndef XAOD_ANALYSIS
DECLARE_COMPONENT( ThinTrkTrackAlg )
#endif
DECLARE_COMPONENT( ThinTRTStandaloneTrackAlg )
