#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../FastCaloSimSvc.h"
#include "../FastCaloSimSvcV2.h"

DECLARE_NAMESPACE_SERVICE_FACTORY( ISF , FastCaloSimSvc )
DECLARE_NAMESPACE_SERVICE_FACTORY( ISF , FastCaloSimSvcV2 )

DECLARE_FACTORY_ENTRIES( ISF_FastCaloSimServices ) {
  DECLARE_NAMESPACE_SERVICE( ISF , FastCaloSimSvc )
  DECLARE_NAMESPACE_SERVICE( ISF , FastCaloSimSvcV2 )
}
