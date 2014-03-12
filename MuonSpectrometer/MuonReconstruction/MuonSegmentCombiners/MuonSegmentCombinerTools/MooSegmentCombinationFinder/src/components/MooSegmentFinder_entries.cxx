#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../MooSegmentCombinationFinder.h"
#include "../MooSegmentFinderAlg.h"
#include "../MuonSegmentFinderAlg.h"
#include "../NSWSectorRoadFinderAlg.h"

DECLARE_ALGORITHM_FACTORY( MooSegmentFinderAlg )
DECLARE_ALGORITHM_FACTORY( MuonSegmentFinderAlg )
DECLARE_ALGORITHM_FACTORY( NSWSectorRoadFinderAlg )
DECLARE_NAMESPACE_TOOL_FACTORY( Muon, MooSegmentCombinationFinder )

DECLARE_FACTORY_ENTRIES( MooSegmentFinder )
{
  DECLARE_ALGORITHM( MooSegmentFinderAlg )
  DECLARE_ALGORITHM( MuonSegmentFinderAlg )
  DECLARE_ALGORITHM( NSWSectorRoadFinderAlg )
  DECLARE_NAMESPACE_TOOL( Muon, MooSegmentCombinationFinder )
}

