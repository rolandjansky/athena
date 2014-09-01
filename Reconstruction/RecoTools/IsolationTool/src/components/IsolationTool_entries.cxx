#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../TrackIsolationTool.h"
#include "../CaloIsolationTool.h"

using namespace xAOD;

DECLARE_TOOL_FACTORY( TrackIsolationTool )
DECLARE_TOOL_FACTORY( CaloIsolationTool )

DECLARE_FACTORY_ENTRIES( IsolationTool )
{
  DECLARE_TOOL( TrackIsolationTool )
  DECLARE_TOOL( CaloIsolationTool )
}

