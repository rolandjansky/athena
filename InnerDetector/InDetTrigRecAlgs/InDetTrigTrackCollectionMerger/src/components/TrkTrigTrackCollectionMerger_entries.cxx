#include "GaudiKernel/DeclareFactoryEntries.h"
#include "InDetTrigTrackCollectionMerger/TrigTrackCollectionMerger.h"
#include "InDetTrigTrackCollectionMerger/SimpleTrigTrackCollMerger.h"
#include "TrkTrack/Track.h"

#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include "TrkToolInterfaces/IPRD_AssociationTool.h"


using namespace Trk;

DECLARE_NAMESPACE_ALGORITHM_FACTORY( Trk, TrigTrackCollectionMerger )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( Trk, SimpleTrigTrackCollMerger )

DECLARE_FACTORY_ENTRIES( InDetTrigTrackCollectionMerger )
{
  DECLARE_NAMESPACE_ALGORITHM( Trk, TrigTrackCollectionMerger )
  DECLARE_NAMESPACE_ALGORITHM( Trk, SimpleTrigTrackCollMerger )
}

