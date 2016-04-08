#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../TrkExtrapolator.h"

DECLARE_NAMESPACE_TOOL_FACTORY( ISF , TrkExtrapolator )

DECLARE_FACTORY_ENTRIES( ISF_TrackingTools ) {
  DECLARE_NAMESPACE_TOOL( ISF ,  TrkExtrapolator )
}
