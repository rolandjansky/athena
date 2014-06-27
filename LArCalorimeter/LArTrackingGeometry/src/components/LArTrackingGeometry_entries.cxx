#include "GaudiKernel/DeclareFactoryEntries.h"
#include "LArTrackingGeometry/LArVolumeBuilder.h"

using namespace LAr;

DECLARE_TOOL_FACTORY( LArVolumeBuilder )

DECLARE_FACTORY_ENTRIES( LArTrackingGeometry )
{
    DECLARE_TOOL( LArVolumeBuilder );
}


