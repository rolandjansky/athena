#include "GaudiKernel/DeclareFactoryEntries.h"
#include "ISF_FastCaloSimServices/FastCaloSimSvc.h"

DECLARE_NAMESPACE_SERVICE_FACTORY( ISF , FastCaloSimSvc )

DECLARE_FACTORY_ENTRIES( ISF_FastCaloSimServices ) {
  DECLARE_NAMESPACE_SERVICE( ISF , FastCaloSimSvc )
}
