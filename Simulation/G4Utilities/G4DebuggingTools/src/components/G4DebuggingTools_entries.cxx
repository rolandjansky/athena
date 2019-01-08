#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../EnergyConservationTestTool.h"
#include "../HyperspaceCatcherTool.h"
#include "../G4AtlantisDumperTool.h"
#include "../VerboseSelectorTool.h"
#include "../CheckActivationTool.h"
#include "../StepNtupleTool.h"
#include "../StepHistogramTool.h"
#include "../VolumeDebuggerTool.h"
#include "../Geant4SetupCheckerTool.h"

DECLARE_TOOL_FACTORY( G4UA::EnergyConservationTestTool )
DECLARE_TOOL_FACTORY( G4UA::HyperspaceCatcherTool )
DECLARE_TOOL_FACTORY( G4UA::G4AtlantisDumperTool )
DECLARE_TOOL_FACTORY( G4UA::VerboseSelectorTool )
DECLARE_TOOL_FACTORY( G4UA::CheckActivationTool )
DECLARE_TOOL_FACTORY( G4UA::StepNtupleTool )
DECLARE_TOOL_FACTORY( G4UA::StepHistogramTool )
DECLARE_TOOL_FACTORY( G4UA::VolumeDebuggerTool )
DECLARE_TOOL_FACTORY( G4UA::Geant4SetupCheckerTool )

DECLARE_FACTORY_ENTRIES( G4DebuggingTools ) {
    DECLARE_TOOL( G4UA::EnergyConservationTestTool )
    DECLARE_TOOL( G4UA::HyperspaceCatcherTool )
    DECLARE_TOOL( G4UA::G4AtlantisDumperTool )
    DECLARE_TOOL( G4UA::VerboseSelectorTool )
    DECLARE_TOOL( G4UA::CheckActivationTool )
    DECLARE_TOOL( G4UA::StepNtupleTool )
    DECLARE_TOOL( G4UA::StepHistogramTool )
    DECLARE_TOOL( G4UA::VolumeDebuggerTool )
    DECLARE_TOOL( G4UA::Geant4SetupCheckerTool )
}
