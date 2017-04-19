// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "InDetTrackSystematicsTools/InDetTrackSmearingTool.h"
#include "InDetTrackSystematicsTools/InDetTrackTruthOriginTool.h"
#include "InDetTrackSystematicsTools/InDetTrackTruthFilterTool.h"
#include "InDetTrackSystematicsTools/InDetTrackBiasingTool.h"
#include "InDetTrackSystematicsTools/JetTrackFilterTool.h"

#include "../InDetTrackSmearingToolTester.h"

DECLARE_NAMESPACE_TOOL_FACTORY( InDet, InDetTrackSmearingTool )
DECLARE_NAMESPACE_TOOL_FACTORY( InDet, InDetTrackTruthOriginTool )
DECLARE_NAMESPACE_TOOL_FACTORY( InDet, InDetTrackTruthFilterTool )
DECLARE_NAMESPACE_TOOL_FACTORY( InDet, InDetTrackBiasingTool )
DECLARE_NAMESPACE_TOOL_FACTORY( InDet, JetTrackFilterTool )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( InDet, InDetTrackSmearingToolTester )

DECLARE_FACTORY_ENTRIES( InDetTrackSystematicsTools ) {

  DECLARE_NAMESPACE_TOOL( InDet, InDetTrackSmearingTool )
  DECLARE_NAMESPACE_TOOL( InDet, InDetTrackTruthOriginTool )
  DECLARE_NAMESPACE_TOOL( InDet, InDetTrackTruthFilterTool )
  DECLARE_NAMESPACE_TOOL( InDet, InDetTrackBiasingTool )
  DECLARE_NAMESPACE_TOOL( InDet, JetTrackFilterTool )
  DECLARE_NAMESPACE_ALGORITHM( InDet, InDetTrackSmearingToolTester );

}
