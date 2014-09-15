#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrkExSTEP_Propagator/STEP_Propagator.h"

using namespace Trk;

DECLARE_TOOL_FACTORY( STEP_Propagator )

/** factory entries need to have the name of the package */
DECLARE_FACTORY_ENTRIES( TrkExSTEP_Propagator )
{
    DECLARE_TOOL( STEP_Propagator )
}

