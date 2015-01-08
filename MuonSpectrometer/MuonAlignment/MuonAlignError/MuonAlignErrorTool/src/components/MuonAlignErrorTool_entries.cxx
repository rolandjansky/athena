#include "MuonAlignErrorTool/MuonAlignErrorExampleAlg.h"
#include "MuonAlignErrorTool/AlignmentErrorTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY( MuonAlign, MuonAlignErrorExampleAlg )
DECLARE_NAMESPACE_TOOL_FACTORY( MuonAlign, AlignmentErrorTool )

DECLARE_FACTORY_ENTRIES(MuonAlignErrorTool) {
  DECLARE_NAMESPACE_ALGORITHM( MuonAlign, MuonAlignErrorExampleAlg )
  DECLARE_NAMESPACE_TOOL( MuonAlign, AlignmentErrorTool )
}
