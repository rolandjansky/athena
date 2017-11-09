
#include "GaudiKernel/DeclareFactoryEntries.h"

#include "PMGTools/PMGCrossSectionTool.h"
#include "PMGTools/PMGSherpa22VJetsWeightTool.h"
#include "PMGTools/PMGSherpaVjetsSysTool.h"
#include "PMGTools/PMGTruthWeightTool.h"

using namespace PMGTools;

DECLARE_TOOL_FACTORY( PMGCrossSectionTool )
DECLARE_TOOL_FACTORY( PMGSherpa22VJetsWeightTool )
DECLARE_TOOL_FACTORY( PMGSherpaVjetsSysTool )
DECLARE_TOOL_FACTORY( PMGTruthWeightTool )

DECLARE_FACTORY_ENTRIES( PMGTools )
{
  DECLARE_TOOL( PMGSherpa22VJetsWeightTool );
  DECLARE_TOOL( PMGSherpaVjetsSysTool );
  DECLARE_TOOL( PMGCrossSectionTool );
  DECLARE_TOOL( PMGTruthWeightTool );
}
