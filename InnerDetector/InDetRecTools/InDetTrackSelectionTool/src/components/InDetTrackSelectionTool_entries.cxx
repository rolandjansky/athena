// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "InDetTrackSelectionTool/InDetTrackSelectionTool.h"

#include "../ToolTester.h"

DECLARE_NAMESPACE_TOOL_FACTORY( InDet, InDetTrackSelectionTool )

DECLARE_NAMESPACE_ALGORITHM_FACTORY( InDet, ToolTester )

DECLARE_FACTORY_ENTRIES( InDetTrackSelectionTool ) {
  
  DECLARE_NAMESPACE_TOOL( InDet, InDetTrackSelectionTool );

  //the athena tool tester library src/ToolTester
  DECLARE_NAMESPACE_ALGORITHM( InDet, ToolTester );
  
}
