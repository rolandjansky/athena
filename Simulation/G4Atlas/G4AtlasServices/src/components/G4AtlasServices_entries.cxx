#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../SensitiveDetectorSvc.h"
#include "../FastSimulationSvc.h"
#include "../UserActionSvc.h"

DECLARE_SERVICE_FACTORY( SensitiveDetectorSvc )
DECLARE_SERVICE_FACTORY( FastSimulationSvc )
DECLARE_SERVICE_FACTORY( UserActionSvc )

DECLARE_FACTORY_ENTRIES( G4AtlasServices ) {
  DECLARE_SERVICE( SensitiveDetectorSvc )
  DECLARE_SERVICE( FastSimulationSvc )
  DECLARE_SERVICE( UserActionSvc )
}
