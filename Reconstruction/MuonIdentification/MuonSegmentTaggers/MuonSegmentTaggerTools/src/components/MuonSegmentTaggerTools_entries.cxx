#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../MuTagMatchingTool.h"
#include "../MuTagAmbiguitySolverTool.h"


DECLARE_TOOL_FACTORY     ( MuTagMatchingTool)
DECLARE_TOOL_FACTORY     ( MuTagAmbiguitySolverTool)

DECLARE_FACTORY_ENTRIES( MuonSegmentTaggerTools ){
      DECLARE_TOOL     ( MuTagMatchingTool   );
      DECLARE_TOOL     ( MuTagAmbiguitySolverTool  );
}

