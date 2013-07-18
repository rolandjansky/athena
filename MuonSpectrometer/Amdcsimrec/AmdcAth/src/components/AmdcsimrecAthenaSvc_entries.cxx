#include "GaudiKernel/DeclareFactoryEntries.h"
#include "AmdcAth/AmdcsimrecAthenaSvc.h"

DECLARE_SERVICE_FACTORY( AmdcsimrecAthenaSvc )
 
DECLARE_FACTORY_ENTRIES(AmdcAth) {
    DECLARE_SERVICE( AmdcsimrecAthenaSvc );
}
 
