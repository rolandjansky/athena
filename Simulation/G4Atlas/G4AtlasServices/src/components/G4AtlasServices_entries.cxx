#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../UserActionSvc.h"

DECLARE_SERVICE_FACTORY( UserActionSvc )

DECLARE_FACTORY_ENTRIES( G4AtlasServices ) {
  DECLARE_SERVICE( UserActionSvc )
}
