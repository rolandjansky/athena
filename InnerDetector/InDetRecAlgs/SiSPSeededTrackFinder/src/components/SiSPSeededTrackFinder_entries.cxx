#include "GaudiKernel/DeclareFactoryEntries.h"
#include "SiSPSeededTrackFinder/SiSPSeededTrackFinder.h"
#include "SiSPSeededTrackFinder/SiSPSeededTrackFinderROI.h"

using namespace InDet;

DECLARE_ALGORITHM_FACTORY( SiSPSeededTrackFinder )

DECLARE_FACTORY_ENTRIES( SiSPSeededTrackFinder )
{
  DECLARE_ALGORITHM( SiSPSeededTrackFinder )
}

DECLARE_ALGORITHM_FACTORY( SiSPSeededTrackFinderROI )

DECLARE_FACTORY_ENTRIES( SiSPSeededTrackFinderROI )
{
  DECLARE_ALGORITHM( SiSPSeededTrackFinderROI )
}
