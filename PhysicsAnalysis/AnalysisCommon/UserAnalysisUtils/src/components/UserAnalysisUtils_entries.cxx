//#include "UserAnalysisUtils/UserMuonTool.h"

#include "UserAnalysisUtils/UserAnalysisPreparationTool.h"
#include "UserAnalysisUtils/UserAnalysisSelectionTool.h"
#include "UserAnalysisUtils/UserAnalysisOverlapCheckingTool.h"
#include "UserAnalysisUtils/UserAnalysisOverlapRemovalTool.h"

#include "GaudiKernel/DeclareFactoryEntries.h"
 
//DECLARE_TOOL_FACTORY( UserMuonTool )

DECLARE_TOOL_FACTORY( UserAnalysisPreparationTool )
DECLARE_TOOL_FACTORY( UserAnalysisSelectionTool )
DECLARE_TOOL_FACTORY( UserAnalysisOverlapCheckingTool )
DECLARE_TOOL_FACTORY( UserAnalysisOverlapRemovalTool )

DECLARE_FACTORY_ENTRIES( UserAnalysisUtils )
{
  //    DECLARE_TOOL( UserMuonTool )

    DECLARE_TOOL( UserAnalysisPreparationTool )
    DECLARE_TOOL( UserAnalysisSelectionTool )
    DECLARE_TOOL( UserAnalysisOverlapCheckingTool )
    DECLARE_TOOL( UserAnalysisOverlapRemovalTool )

}


