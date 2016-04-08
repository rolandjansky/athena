#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../TestPixelModuleDisablingSvc.h"


DECLARE_SERVICE_FACTORY( TestPixelModuleDisablingSvc )

DECLARE_FACTORY_ENTRIES( TrigIDUtils )
{
  DECLARE_SERVICE( TestPixelModuleDisablingSvc )
}

