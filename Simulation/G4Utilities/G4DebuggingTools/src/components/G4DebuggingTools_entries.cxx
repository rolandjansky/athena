#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../VolumeDebugger.h"
#include "../G4AtlantisDumper.h"
#include "../VerboseSelector.h"
#include "../StepNtuple.h"
#include "../EnergyConservationTest.h"
#include "../HyperspaceCatcher.h"

DECLARE_TOOL_FACTORY( VolumeDebugger )
DECLARE_TOOL_FACTORY( G4AtlantisDumper )
DECLARE_TOOL_FACTORY( VerboseSelector )
DECLARE_TOOL_FACTORY( StepNtuple )
DECLARE_TOOL_FACTORY( EnergyConservationTest )
DECLARE_TOOL_FACTORY( HyperspaceCatcher )

DECLARE_FACTORY_ENTRIES( G4DebuggingTools ) {

  DECLARE_TOOL( VolumeDebugger )
  DECLARE_TOOL( G4AtlantisDumper )
  DECLARE_TOOL( VerboseSelector )
  DECLARE_TOOL( StepNtuple )
  DECLARE_TOOL( EnergyConservationTest )
  DECLARE_TOOL( HyperspaceCatcher )

}
