#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../SegmentTimingTool.h"
#include "../RPC_TimingTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY( Muon, SegmentTimingTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Muon, RPC_TimingTool )

DECLARE_FACTORY_ENTRIES( SegmentTimingTool ) {
  DECLARE_NAMESPACE_TOOL( Muon, SegmentTimingTool)
  DECLARE_NAMESPACE_TOOL( Muon, RPC_TimingTool )
}
