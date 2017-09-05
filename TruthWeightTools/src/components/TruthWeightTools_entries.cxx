#include "GaudiKernel/DeclareFactoryEntries.h"

#include "TruthWeightTools/TruthWeightTool.h"

using namespace xAOD;

DECLARE_TOOL_FACTORY( TruthWeightTool )

DECLARE_FACTORY_ENTRIES( TruthWeightTools ) {

  DECLARE_TOOL( TruthWeightTool )

}
