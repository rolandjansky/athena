#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrkVertexSeedFinderTools/MCTrueSeedFinder.h"
#include "TrkVertexSeedFinderTools/CrossDistancesSeedFinder.h"
#include "TrkVertexSeedFinderTools/DummySeedFinder.h"
#include "TrkVertexSeedFinderTools/ZScanSeedFinder.h"
#include "TrkVertexSeedFinderTools/ImagingSeedFinder.h"
#include "TrkVertexSeedFinderTools/TrackDensitySeedFinder.h"

using namespace Trk ;

DECLARE_COMPONENT( MCTrueSeedFinder )
DECLARE_COMPONENT( CrossDistancesSeedFinder )
DECLARE_COMPONENT( DummySeedFinder )
DECLARE_COMPONENT( ZScanSeedFinder )
DECLARE_COMPONENT( ImagingSeedFinder )
DECLARE_COMPONENT( TrackDensitySeedFinder )

DECLARE_FACTORY_ENTRIES( TrkVertexSeedFinderTools )
{
  DECLARE_TOOL( MCTrueSeedFinder );
  DECLARE_TOOL( CrossDistancesSeedFinder );
  DECLARE_TOOL( DummySeedFinder );
  DECLARE_TOOL( ZScanSeedFinder );
  DECLARE_TOOL( ImagingSeedFinder );
}
