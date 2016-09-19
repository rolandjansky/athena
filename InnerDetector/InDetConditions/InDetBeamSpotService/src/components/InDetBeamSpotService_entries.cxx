#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../BeamCondSvc.h"

DECLARE_SERVICE_FACTORY( BeamCondSvc )

DECLARE_FACTORY_ENTRIES( InDetBeamSpotService ) {
  DECLARE_SERVICE( BeamCondSvc )
}

