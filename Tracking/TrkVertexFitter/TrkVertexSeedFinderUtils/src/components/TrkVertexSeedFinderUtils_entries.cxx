#include "../GaussianDensityTestAlg.h"
#include "TrkVertexSeedFinderUtils/FsmwMode1dFinder.h"
#include "TrkVertexSeedFinderUtils/GaussianTrackDensity.h"
#include "TrkVertexSeedFinderUtils/Mode3dFromFsmw1dFinder.h"
#include "TrkVertexSeedFinderUtils/Mode3dTo1dFinder.h"
#include "TrkVertexSeedFinderUtils/NewtonTrkDistanceFinder.h"
#include "TrkVertexSeedFinderUtils/SeedNewtonTrkDistanceFinder.h"
#include "TrkVertexSeedFinderUtils/Trk2DDistanceFinder.h"
#include "TrkVertexSeedFinderUtils/Trk2dDistanceSeeder.h"

using namespace Trk ;

DECLARE_COMPONENT( Trk2dDistanceSeeder )
DECLARE_COMPONENT( NewtonTrkDistanceFinder )
DECLARE_COMPONENT( Mode3dTo1dFinder )
DECLARE_COMPONENT( SeedNewtonTrkDistanceFinder )
DECLARE_COMPONENT( FsmwMode1dFinder )
DECLARE_COMPONENT( Trk2DDistanceFinder )
DECLARE_COMPONENT( Mode3dFromFsmw1dFinder )
DECLARE_COMPONENT( GaussianTrackDensity )
DECLARE_COMPONENT( GaussianDensityTestAlg )
