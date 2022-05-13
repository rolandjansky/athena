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
// AthAnalysisBase doesn't know about Trk::Tracks
#ifndef XAOD_ANALYSIS
DECLARE_COMPONENT( ThinTrkTrackAlg )
#endif
DECLARE_COMPONENT( ThinTRTStandaloneTrackAlg )
