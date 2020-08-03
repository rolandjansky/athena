#include "GaudiKernel/DeclareFactoryEntries.h"
#include "SiSPSeededTrackFinder/SiSPSeededTrackFinder.h"
#include "SiSPSeededTrackFinder/SiSPSeededTrackFinderRoI.h"

using namespace InDet;

DECLARE_ALGORITHM_FACTORY( SiSPSeededTrackFinder )
DECLARE_ALGORITHM_FACTORY( SiSPSeededTrackFinderRoI )

DECLARE_FACTORY_ENTRIES( SiSPSeededTrackFinder )
{
  DECLARE_ALGORITHM( SiSPSeededTrackFinder )
  DECLARE_ALGORITHM( SiSPSeededTrackFinderRoI )
}

