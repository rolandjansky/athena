#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../TrackIsolationTool.h"
#include "../CaloIsolationTool.h"
#include "../egammaIsoPtCorrection.h"

using namespace xAOD;

DECLARE_TOOL_FACTORY( TrackIsolationTool )
DECLARE_TOOL_FACTORY( CaloIsolationTool )
DECLARE_TOOL_FACTORY( egammaIsoPtCorrection )

DECLARE_FACTORY_ENTRIES( IsolationTool )
{
  DECLARE_TOOL( TrackIsolationTool )
  DECLARE_TOOL( CaloIsolationTool )
  DECLARE_TOOL( egammaIsoPtCorrection )
}

