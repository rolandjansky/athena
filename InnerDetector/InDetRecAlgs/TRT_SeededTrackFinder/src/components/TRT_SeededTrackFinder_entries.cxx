#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TRT_SeededTrackFinder/TRT_SeededTrackFinder.h"

using namespace InDet;

DECLARE_ALGORITHM_FACTORY( TRT_SeededTrackFinder )

DECLARE_FACTORY_ENTRIES( TRT_SeededTrackFinder )
{
  DECLARE_ALGORITHM( TRT_SeededTrackFinder )
}

