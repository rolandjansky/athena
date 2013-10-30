#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrkVertexSeedFinderUtils/Trk2dDistanceSeeder.h"
#include "TrkVertexSeedFinderUtils/NewtonTrkDistanceFinder.h"
#include "TrkVertexSeedFinderUtils/FsmwMode1dFinder.h"
#include "TrkVertexSeedFinderUtils/Mode3dTo1dFinder.h"
#include "TrkVertexSeedFinderUtils/SeedNewtonTrkDistanceFinder.h"
#include "TrkVertexSeedFinderUtils/Trk2DDistanceFinder.h"

using namespace Trk ;

DECLARE_TOOL_FACTORY( Trk2dDistanceSeeder )
DECLARE_TOOL_FACTORY( NewtonTrkDistanceFinder )
DECLARE_TOOL_FACTORY( Mode3dTo1dFinder )
DECLARE_TOOL_FACTORY( SeedNewtonTrkDistanceFinder )
DECLARE_TOOL_FACTORY( FsmwMode1dFinder )
DECLARE_TOOL_FACTORY( Trk2DDistanceFinder )

/** factory entries need to have the name of the package */
DECLARE_FACTORY_ENTRIES( TrkVertexSeedFinderUtils )
{

  DECLARE_TOOL( Trk2dDistanceSeeder );
  DECLARE_TOOL( NewtonTrkDistanceFinder );
  DECLARE_TOOL( Mode3dTo1dFinder );
  DECLARE_TOOL( SeedNewtonTrkDistanceFinder );
  DECLARE_TOOL( FsmwMode1dFinder );
  DECLARE_TOOL( Trk2DDistanceFinderpwd
 );
  
}
