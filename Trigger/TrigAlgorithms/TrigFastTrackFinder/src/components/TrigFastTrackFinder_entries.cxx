#include "GaudiKernel/DeclareFactoryEntries.h"

#include "TrigFastTrackFinder/TrigFastTrackFinder.h"
#include "../TrigFastTrackFinderMT.h"

DECLARE_ALGORITHM_FACTORY( TrigFastTrackFinder)
DECLARE_ALGORITHM_FACTORY( TrigFastTrackFinderMT)
DECLARE_FACTORY_ENTRIES( TrigFastTrackFinder )
{
  DECLARE_ALGORITHM( TrigFastTrackFinder )
  DECLARE_ALGORITHM( TrigFastTrackFinderMT )
}
