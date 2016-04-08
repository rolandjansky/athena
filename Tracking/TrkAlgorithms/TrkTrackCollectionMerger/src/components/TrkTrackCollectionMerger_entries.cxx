#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrkTrackCollectionMerger/TrackCollectionMerger.h"

using namespace Trk;

DECLARE_ALGORITHM_FACTORY( TrackCollectionMerger )

DECLARE_FACTORY_ENTRIES( TrkTrackCollectionMerger )
{
  DECLARE_ALGORITHM( TrackCollectionMerger )
}

