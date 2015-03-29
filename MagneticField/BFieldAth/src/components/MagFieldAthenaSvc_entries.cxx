#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../MagFieldAthenaSvc.h"

DECLARE_SERVICE_FACTORY( MagFieldAthenaSvc )
 
DECLARE_FACTORY_ENTRIES(BFieldAth) {
    DECLARE_SERVICE( MagFieldAthenaSvc )
}
 
