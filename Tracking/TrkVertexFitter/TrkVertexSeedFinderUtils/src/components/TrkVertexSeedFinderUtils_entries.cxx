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

DECLARE_TOOL_FACTORY( Trk2dDistanceSeeder )
DECLARE_TOOL_FACTORY( NewtonTrkDistanceFinder )
DECLARE_TOOL_FACTORY( Mode3dTo1dFinder )
DECLARE_TOOL_FACTORY( SeedNewtonTrkDistanceFinder )
DECLARE_TOOL_FACTORY( FsmwMode1dFinder )
DECLARE_TOOL_FACTORY( Trk2DDistanceFinder )
DECLARE_TOOL_FACTORY( SimpleVertexClusterFinder )
DECLARE_TOOL_FACTORY( LocalMax1DClusterFinder )
DECLARE_TOOL_FACTORY( VertexImageMaker )
DECLARE_ALGORITHM_FACTORY( ImagingSeedTuningAlg )

/** factory entries need to have the name of the package */

