#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../VolumeDebugger.h"
#include "../G4AtlantisDumper.h"
#include "../VerboseSelector.h"
#include "../StepNtuple.h"
#include "../EnergyConservationTest.h"
#include "../HyperspaceCatcher.h"

// New MT tools

#include "../EnergyConservationTestTool.h"
#include "../HyperspaceCatcherTool.h"
#include "../G4AtlantisDumperTool.h"
#include "../VerboseSelectorTool.h"
#include "../CheckActivationTool.h"

DECLARE_TOOL_FACTORY( G4UA::EnergyConservationTestTool )
DECLARE_TOOL_FACTORY( G4UA::HyperspaceCatcherTool )
DECLARE_TOOL_FACTORY( G4UA::G4AtlantisDumperTool )
DECLARE_TOOL_FACTORY( G4UA::VerboseSelectorTool )
DECLARE_TOOL_FACTORY( G4UA::CheckActivationTool )

// old tools

DECLARE_TOOL_FACTORY( VolumeDebugger )
DECLARE_TOOL_FACTORY( G4AtlantisDumper )
DECLARE_TOOL_FACTORY( VerboseSelector )
DECLARE_TOOL_FACTORY( StepNtuple )
DECLARE_TOOL_FACTORY( EnergyConservationTest )
DECLARE_TOOL_FACTORY( HyperspaceCatcher )

DECLARE_FACTORY_ENTRIES( G4DebuggingTools ) {
  // old tools
  DECLARE_TOOL( VolumeDebugger )
    DECLARE_TOOL( G4AtlantisDumper )
    DECLARE_TOOL( VerboseSelector )
    DECLARE_TOOL( StepNtuple )
    DECLARE_TOOL( EnergyConservationTest )
    DECLARE_TOOL( HyperspaceCatcher )
    
    // new tools
    DECLARE_TOOL( G4UA::EnergyConservationTestTool )
    DECLARE_TOOL( G4UA::HyperspaceCatcherTool )
    DECLARE_TOOL( G4UA::G4AtlantisDumperTool )
    DECLARE_TOOL( G4UA::VerboseSelectorTool )
    DECLARE_TOOL( G4UA::CheckActivationTool )
}
