#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../RPC_TimingTool.h"
#include "../MuonHitTimingTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY( Muon, RPC_TimingTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Muon, MuonHitTimingTool )

DECLARE_FACTORY_ENTRIES( SegmentTimingTool ) {
  DECLARE_NAMESPACE_TOOL( Muon, RPC_TimingTool )
  DECLARE_NAMESPACE_TOOL( Muon, MuonHitTimingTool )
}
