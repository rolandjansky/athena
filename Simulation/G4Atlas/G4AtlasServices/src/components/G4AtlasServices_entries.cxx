#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../G4AtlasSvc.h"
#include "../G4GeometryNotifierSvc.h"
#include "../DetectorGeometrySvc.h"
#include "../UserActionSvc.h"
#include "../ConstantFieldSvc.h"
#include "../StandardFieldSvc.h"

DECLARE_SERVICE_FACTORY( G4AtlasSvc )
DECLARE_SERVICE_FACTORY( G4GeometryNotifierSvc )
DECLARE_SERVICE_FACTORY( DetectorGeometrySvc )
DECLARE_SERVICE_FACTORY( G4UA::UserActionSvc )
DECLARE_SERVICE_FACTORY( ConstantFieldSvc )
DECLARE_SERVICE_FACTORY( StandardFieldSvc )
