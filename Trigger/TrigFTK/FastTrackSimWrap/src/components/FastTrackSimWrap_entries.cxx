#include "GaudiKernel/DeclareFactoryEntries.h"
//#include "GaudiKernel/ServiceHandle.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"
#include "FastTrackSimWrap/DumpSp.h"
#include "FastTrackSimWrap/FTKRegionalWrapper.h"

DECLARE_ALGORITHM_FACTORY( DumpSp )
DECLARE_ALGORITHM_FACTORY( FTKRegionalWrapper )
DECLARE_FACTORY_ENTRIES( FastTrackSimWrap ) 
{
    DECLARE_ALGORITHM( DumpSp );
    DECLARE_ALGORITHM( FTKRegionalWrapper );
}

