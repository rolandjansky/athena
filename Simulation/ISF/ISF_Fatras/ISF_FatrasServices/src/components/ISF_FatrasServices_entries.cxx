#include "GaudiKernel/DeclareFactoryEntries.h"
#include "ISF_FatrasServices/FatrasSimSvc.h"

DECLARE_NAMESPACE_SERVICE_FACTORY( iFatras , FatrasSimSvc )

DECLARE_FACTORY_ENTRIES( ISF_FatrasServices ) {
  DECLARE_NAMESPACE_SERVICE( iFatras ,  FatrasSimSvc )
}   
