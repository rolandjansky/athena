#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MuonErrorScalingTools/MuonRIO_OnTrackErrorScalingTool.h"
#include "MuonErrorScalingTools/MuonErrorScaleDbTool.h"

using namespace Muon;

DECLARE_TOOL_FACTORY( MuonRIO_OnTrackErrorScalingTool )
DECLARE_TOOL_FACTORY( MuonErrorScaleDbTool )

DECLARE_FACTORY_ENTRIES( MuonErrorScalingTools )
{
  DECLARE_TOOL( MuonRIO_OnTrackErrorScalingTool )
  DECLARE_TOOL( MuonErrorScaleDbTool )
}
