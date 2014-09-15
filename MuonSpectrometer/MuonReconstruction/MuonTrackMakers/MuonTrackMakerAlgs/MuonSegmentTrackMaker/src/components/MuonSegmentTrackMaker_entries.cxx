#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MuonSegmentTrackMaker/MuonCombiTrackBuilder.h"
#include "MuonSegmentTrackMaker/MuPatTrackBuilder.h"

DECLARE_ALGORITHM_FACTORY( MuonCombiTrackBuilder )
DECLARE_ALGORITHM_FACTORY( MuPatTrackBuilder )

DECLARE_FACTORY_ENTRIES( MuonSegmentTrackMaker ) 
{
  DECLARE_ALGORITHM( MuonCombiTrackBuilder );
  DECLARE_ALGORITHM( MuPatTrackBuilder );
}
