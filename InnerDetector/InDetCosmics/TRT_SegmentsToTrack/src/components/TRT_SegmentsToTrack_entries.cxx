#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TRT_SegmentsToTrack/TRT_SegmentsToTrack.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY( InDet, TRT_SegmentsToTrack )

DECLARE_FACTORY_ENTRIES( TRT_SegmentsToTrack ){
  DECLARE_NAMESPACE_ALGORITHM( InDet, TRT_SegmentsToTrack )
}


