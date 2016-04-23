// $Id$

#include "GaudiKernel/DeclareFactoryEntries.h"

#include "xAODBTaggingEfficiency/BTaggingEfficiencyTool.h"
#include "xAODBTaggingEfficiency/BTaggingSelectionTool.h"

#include "../ToolTester.h"
// Should probably alter the namespace 

DECLARE_TOOL_FACTORY( BTaggingEfficiencyTool)
DECLARE_TOOL_FACTORY( BTaggingSelectionTool)

DECLARE_ALGORITHM_FACTORY( BTagToolTester )

DECLARE_FACTORY_ENTRIES( xAODBTaggingEfficiency ) {

  DECLARE_TOOL(BTaggingEfficiencyTool )
    DECLARE_TOOL(BTaggingSelectionTool )
    DECLARE_ALGORITHM(BTagToolTester )

}
