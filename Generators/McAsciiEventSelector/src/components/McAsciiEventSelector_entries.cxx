#include "../McAsciiCnvSvc.h"
#include "../McAsciiEventCnv.h"
#include "../McAsciiEventSelector.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_CONVERTER_FACTORY( McAsciiEventCnv )
DECLARE_SERVICE_FACTORY( McAsciiCnvSvc )
DECLARE_SERVICE_FACTORY( McAsciiEventSelector )

DECLARE_FACTORY_ENTRIES( McAsciiEventSelector ) {
  DECLARE_CONVERTER( McAsciiEventCnv )
  DECLARE_SERVICE( McAsciiCnvSvc )
  DECLARE_SERVICE( McAsciiEventSelector )
}
