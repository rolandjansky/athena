#include "GaudiKernel/DeclareFactoryEntries.h"

#include "MCTruthBase/MCTruthSteppingAction.h"

DECLARE_TOOL_FACTORY( MCTruthSteppingAction )

DECLARE_FACTORY_ENTRIES( MCTruthBase ) {
  DECLARE_TOOL( MCTruthSteppingAction )
}
