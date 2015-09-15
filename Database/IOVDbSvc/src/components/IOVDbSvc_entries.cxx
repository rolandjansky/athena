#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../IOVDbSvc.h"

DECLARE_SERVICE_FACTORY(IOVDbSvc)
DECLARE_FACTORY_ENTRIES(IOVDbSvc) {
  DECLARE_SERVICE( IOVDbSvc );
}
