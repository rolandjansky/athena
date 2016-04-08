#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../G4AtlasSvc.h"
#include "../G4GeometryNotifierSvc.h"
#include "../DetectorGeometrySvc.h"
#include "../UserActionSvc.h"

DECLARE_SERVICE_FACTORY( G4AtlasSvc )
DECLARE_SERVICE_FACTORY( G4GeometryNotifierSvc )
DECLARE_SERVICE_FACTORY( DetectorGeometrySvc )
DECLARE_SERVICE_FACTORY( UserActionSvc )

// DECLARE_FACTORY_ENTRIES( G4AtlasServices ) {
//   DECLARE_SERVICE( G4AtlasSvc )
//   DECLARE_SERVICE( DetectorGeometrySvc )
//   DECLARE_SERVICE( UserActionSvc )
// }
