#include "GaudiKernel/DeclareFactoryEntries.h"

#include "TrkG4UserActions/MaterialStepRecorderTool.h"

#include "TrkG4UserActions/EnergyLossRecorderTool.h"
#include "TrkG4UserActions/GeantFollowerTool.h"

DECLARE_TOOL_FACTORY( G4UA::MaterialStepRecorderTool )
//DECLARE_TOOL_FACTORY( G4UA::EnergyLossRecorderTool )
//DECLARE_TOOL_FACTORY( G4UA::GeantFollowerTool )

DECLARE_FACTORY_ENTRIES( TrkG4UserActions ) {
    DECLARE_TOOL( G4UA::MaterialStepRecorderTool )
      //    DECLARE_TOOL( G4UA::EnergyLossRecorderTool )
      //    DECLARE_TOOL( G4UA::GeantFollowerTool )
}
