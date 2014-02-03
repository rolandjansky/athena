#include "LArG4ShowerLibSvc/LArG4ShowerLibSvcTest.h"
#include "LArG4ShowerLibSvc/LArG4ShowerLibSvc.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY ( LArG4ShowerLibSvcTest )

DECLARE_SERVICE_FACTORY   ( LArG4ShowerLibSvc )

DECLARE_FACTORY_ENTRIES( LArG4ShowerLibSvc ) {
    DECLARE_SERVICE   ( LArG4ShowerLibSvc )
    DECLARE_ALGORITHM ( LArG4ShowerLibSvcTest )
}
