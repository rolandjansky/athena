#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../MuonClusterSegmentFinder.h"

using namespace Muon;

DECLARE_TOOL_FACTORY( MuonClusterSegmentFinder )

DECLARE_FACTORY_ENTRIES( MuonClusterSegmentMakerTools )
{

  DECLARE_TOOL( MuonClusterSegmentFinder )
}
