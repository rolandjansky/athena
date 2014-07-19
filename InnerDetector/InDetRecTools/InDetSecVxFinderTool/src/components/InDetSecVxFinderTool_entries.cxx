#include "GaudiKernel/DeclareFactoryEntries.h"
#include "InDetSecVxFinderTool/InDetJetFitterVxFinder.h"
#include "InDetSecVxFinderTool/InDetJetFitterUtils.h"
#include "InDetSecVxFinderTool/InDetImprovedJetFitterVxFinder.h"

using namespace InDet;

DECLARE_TOOL_FACTORY( InDetJetFitterVxFinder )
DECLARE_TOOL_FACTORY( InDetJetFitterUtils )
DECLARE_TOOL_FACTORY( InDetImprovedJetFitterVxFinder )

DECLARE_FACTORY_ENTRIES( InDetSecVxFinderTool )
{
  DECLARE_TOOL( InDetJetFitterVxFinder );
  DECLARE_TOOL( InDetJetFitterUtils );
  DECLARE_TOOL( InDetImprovedJetFitterVxFinder );
}
