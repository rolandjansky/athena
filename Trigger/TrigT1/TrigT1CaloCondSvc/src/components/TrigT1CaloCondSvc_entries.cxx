#include "GaudiKernel/DeclareFactoryEntries.h"

#include "TrigT1CaloCondSvc/L1CaloCondSvc.h"

DECLARE_SERVICE_FACTORY( L1CaloCondSvc )

DECLARE_FACTORY_ENTRIES( TrigT1CaloCondSvc ) {
  DECLARE_SERVICE( L1CaloCondSvc )
}
