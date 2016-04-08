#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TRT_TrigTrackSegmentsFinder/TRT_TrigTrackSegmentsFinder.h"
#include "IRegionSelector/IRegSelSvc.h"

using namespace InDet;

DECLARE_ALGORITHM_FACTORY( TRT_TrigTrackSegmentsFinder )

DECLARE_FACTORY_ENTRIES( TRT_TrigTrackSegmentsFinder )
{
  DECLARE_ALGORITHM( TRT_TrigTrackSegmentsFinder )
}

