#include "TrkVertexFitterUtils/KalmanVertexUpdator.h"
#include "TrkVertexFitterUtils/KalmanVertexTrackUpdator.h"
#include "TrkVertexFitterUtils/FullLinearizedTrackFactory.h"
#include "TrkVertexFitterUtils/Chi2TrackCompatibilityEstimator.h"
#include "TrkVertexFitterUtils/DetAnnealingMaker.h"
#include "TrkVertexFitterUtils/DummyAnnealingMaker.h"
#include "src/ImpactPoint3dEstimator.h"
#include "src/NeutralParticleParameterCalculator.h"
#include "TrkVertexFitterUtils/TrackToVertexIPEstimator.h"
#include "TrkVertexFitterUtils/Z0PVTrackCompatibilityEstimator.h"


using namespace Trk;

DECLARE_COMPONENT( KalmanVertexUpdator )
DECLARE_COMPONENT( KalmanVertexTrackUpdator )
DECLARE_COMPONENT( FullLinearizedTrackFactory )
DECLARE_COMPONENT( DummyAnnealingMaker )
DECLARE_COMPONENT( Chi2TrackCompatibilityEstimator )
DECLARE_COMPONENT( DetAnnealingMaker )
DECLARE_COMPONENT( ImpactPoint3dEstimator )
DECLARE_COMPONENT( NeutralParticleParameterCalculator )
DECLARE_COMPONENT( TrackToVertexIPEstimator )
DECLARE_COMPONENT( Z0PVTrackCompatibilityEstimator )

