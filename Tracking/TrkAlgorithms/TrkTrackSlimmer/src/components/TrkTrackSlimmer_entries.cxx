#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrkTrackSlimmer/TrackSlimmer.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY( Trk , TrackSlimmer )

DECLARE_FACTORY_ENTRIES( TrkTrackSlimmer ) {
  DECLARE_NAMESPACE_ALGORITHM( Trk ,  TrackSlimmer )
}   
