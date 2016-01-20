#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrkDigUnitTests/DigitizationModuleTest.h"

using namespace Trk;

DECLARE_ALGORITHM_FACTORY( DigitizationModuleTest)

/** factory entries need to have the name of the package */
DECLARE_FACTORY_ENTRIES( TrkDigUnitTests )
{
    DECLARE_ALGORITHM( DigitizationModuleTest )
}
