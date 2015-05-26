#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrkExRungeKuttaPropagator/RungeKuttaPropagator.h"

using namespace Trk;

DECLARE_TOOL_FACTORY( RungeKuttaPropagator )

/** factory entries need to have the name of the package */
DECLARE_FACTORY_ENTRIES( TrkExRungeKuttaPropagator )
{
    DECLARE_TOOL( RungeKuttaPropagator )
}

