#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrigNavigation/Navigation.h"


DECLARE_NAMESPACE_TOOL_FACTORY( HLT, Navigation )


DECLARE_FACTORY_ENTRIES(TrigNavigation) {
  DECLARE_NAMESPACE_ALGTOOL(HLT, Navigation)
}
