#include "TrkVertexSeedFinderUtils/Trk2dDistanceSeeder.h"
#include "TrkVertexSeedFinderUtils/NewtonTrkDistanceFinder.h"
#include "TrkVertexSeedFinderUtils/FsmwMode1dFinder.h"
#include "TrkVertexSeedFinderUtils/Mode3dTo1dFinder.h"
#include "TrkVertexSeedFinderUtils/SeedNewtonTrkDistanceFinder.h"
#include "TrkVertexSeedFinderUtils/Trk2DDistanceFinder.h"
#include "TrkVertexSeedFinderUtils/SimpleVertexClusterFinder.h"
#include "TrkVertexSeedFinderUtils/LocalMax1DClusterFinder.h"
#include "TrkVertexSeedFinderUtils/VertexImageMaker.h"
#include "TrkVertexSeedFinderUtils/ImagingSeedTuningAlg.h"

using namespace Trk ;

DECLARE_COMPONENT( Trk2dDistanceSeeder )
DECLARE_COMPONENT( NewtonTrkDistanceFinder )
DECLARE_COMPONENT( Mode3dTo1dFinder )
DECLARE_COMPONENT( SeedNewtonTrkDistanceFinder )
DECLARE_COMPONENT( FsmwMode1dFinder )
DECLARE_COMPONENT( Trk2DDistanceFinder )
DECLARE_COMPONENT( SimpleVertexClusterFinder )
DECLARE_COMPONENT( LocalMax1DClusterFinder )
DECLARE_COMPONENT( VertexImageMaker )
DECLARE_COMPONENT( ImagingSeedTuningAlg )

