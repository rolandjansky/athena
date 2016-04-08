#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TRT_TrigSeededTrackFinder/TRT_TrigSeededTrackFinder.h"
#include "IRegionSelector/IRegSelSvc.h"

using namespace InDet;

DECLARE_ALGORITHM_FACTORY( TRT_TrigSeededTrackFinder )

DECLARE_FACTORY_ENTRIES( TRT_TrigSeededTrackFinder )
{
  DECLARE_ALGORITHM( TRT_TrigSeededTrackFinder )
}

