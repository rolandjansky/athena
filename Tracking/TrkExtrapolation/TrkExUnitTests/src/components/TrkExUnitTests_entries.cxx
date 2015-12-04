#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrkExUnitTests/ExtrapolationEngineTest.h"

using namespace Trk;

DECLARE_ALGORITHM_FACTORY( ExtrapolationEngineTest )

/** factory entries need to have the name of the package */
DECLARE_FACTORY_ENTRIES( TrkExUnitTests )
{
    DECLARE_ALGORITHM( ExtrapolationEngineTest )
}
