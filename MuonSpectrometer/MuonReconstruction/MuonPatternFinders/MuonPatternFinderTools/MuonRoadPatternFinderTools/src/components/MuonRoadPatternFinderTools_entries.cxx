#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../Muon_CDoAllowCellTool.h"
#include "../Muon_CDoClustersTool.h"
#include "../Muon_CDoErrorSegmentTool.h"
#include "../Muon_CDoStrictSegmentsTool.h"
#include "../Muon_CDoLooseSegmentsTool.h"
#include "../Muon_CDoMDTROATool.h"
#include "../Muon_CDoCSCSegmentsTool.h"
#include "../MuonRoadChamberAssocTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY( Muon, Muon_CDoAllowCellTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Muon, Muon_CDoClustersTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Muon, Muon_CDoCSCSegmentsTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Muon, Muon_CDoMDTROATool )
DECLARE_NAMESPACE_TOOL_FACTORY( Muon, Muon_CDoLooseSegmentsTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Muon, Muon_CDoStrictSegmentsTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Muon, Muon_CDoErrorSegmentTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Muon, MuonRoadChamberAssocTool )


DECLARE_FACTORY_ENTRIES( Muon_CDoAllowCellTool )
{
  DECLARE_NAMESPACE_TOOL( Muon, Muon_CDoAllowCellTool )
}

DECLARE_FACTORY_ENTRIES( Muon_CDoClustersTool )
{
  DECLARE_NAMESPACE_TOOL( Muon, Muon_CDoClustersTool )
}

DECLARE_FACTORY_ENTRIES( Muon_CDoCSCSegmentsTool )
{
  DECLARE_NAMESPACE_TOOL( Muon, Muon_CDoCscSegmentsTool )
}

DECLARE_FACTORY_ENTRIES( Muon_CDoMDTROATool )
{
  DECLARE_NAMESPACE_TOOL( Muon, Muon_CDoMDTROATool )
}

DECLARE_FACTORY_ENTRIES( Muon_CDoLooseSegmentsTool )
{
  DECLARE_NAMESPACE_TOOL( Muon, Muon_CDoLooseSegmentsTool )
}

DECLARE_FACTORY_ENTRIES( Muon_CDoStrictSegmentsTool )
{
  DECLARE_NAMESPACE_TOOL( Muon, Muon_CDoStrictSegmentsTool )
}

DECLARE_FACTORY_ENTRIES( Muon_CDoErrorSegmentTool )
{
  DECLARE_NAMESPACE_TOOL( Muon, Muon_CDoErrorSegmentTool )
}
DECLARE_FACTORY_ENTRIES( MuonRoadPatternFinderTools )
{
  DECLARE_NAMESPACE_TOOL( Muon, MuonRoadChamberAssocTool )
}

