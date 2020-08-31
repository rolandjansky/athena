// $Id$

#include "GaudiKernel/DeclareFactoryEntries.h"

#include "xAODBTaggingEfficiency/BTaggingEfficiencyTool.h"
#include "xAODBTaggingEfficiency/BTaggingSelectionTool.h"
#include "xAODBTaggingEfficiency/BTaggingTruthTaggingTool.h"
#include "xAODBTaggingEfficiency/BTaggingEigenVectorRecompositionTool.h"


#include "../ToolTester.h"
// Should probably alter the namespace

DECLARE_TOOL_FACTORY( BTaggingEfficiencyTool)
DECLARE_TOOL_FACTORY( BTaggingTruthTaggingTool)
DECLARE_TOOL_FACTORY( BTaggingSelectionTool)
DECLARE_TOOL_FACTORY( BTaggingEigenVectorRecompositionTool)


DECLARE_ALGORITHM_FACTORY( BTagToolTester )

DECLARE_FACTORY_ENTRIES( xAODBTaggingEfficiency ) {

  	DECLARE_TOOL(BTaggingEfficiencyTool )
    DECLARE_TOOL(BTaggingTruthTaggingTool )
    DECLARE_TOOL(BTaggingSelectionTool )
    DECLARE_TOOL(BTaggingEigenVectorRecompositionTool )
    DECLARE_ALGORITHM(BTagToolTester )

}
