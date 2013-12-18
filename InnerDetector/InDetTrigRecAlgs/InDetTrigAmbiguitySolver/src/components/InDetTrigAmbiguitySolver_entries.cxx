#include "GaudiKernel/DeclareFactoryEntries.h"
#include "InDetTrigAmbiguitySolver/InDetTrigAmbiguitySolver.h"
#include "TrkTrack/Track.h"

using namespace InDet;

DECLARE_ALGORITHM_FACTORY( InDetTrigAmbiguitySolver )

DECLARE_FACTORY_ENTRIES( InDetTrigAmbiguitySolver )
{
	DECLARE_ALGORITHM( InDetTrigAmbiguitySolver )
}





