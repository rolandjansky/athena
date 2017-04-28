
#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../AthExample.h"
#include <QuickAna/QuickAna.h>

DECLARE_ALGORITHM_FACTORY( ana::AthExample )
DECLARE_TOOL_FACTORY( ana::QuickAna )

DECLARE_FACTORY_ENTRIES( QuickAna )
{
  DECLARE_ALGORITHM( ana::AthExample );
  DECLARE_TOOL( ana::QuickAna );
}
