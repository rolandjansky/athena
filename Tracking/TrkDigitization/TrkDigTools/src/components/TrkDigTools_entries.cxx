#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrkDigTools/PlanarModuleStepper.h"

using namespace Trk;

DECLARE_TOOL_FACTORY( PlanarModuleStepper )

/** factory entries need to have the name of the package */
DECLARE_FACTORY_ENTRIES( TrkDigTools )
{
    DECLARE_TOOL( PlanarModuleStepper )
}
