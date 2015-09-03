#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MuonPrepRawDataProviderTools/MuonLayerHashProviderTool.h"
#include "MuonPrepRawDataProviderTools/MuonPrepRawDataCollectionProviderTool.h"
#include "../MuonPRDSelectionTool.h"

using namespace Muon;

DECLARE_TOOL_FACTORY( MuonLayerHashProviderTool )
DECLARE_TOOL_FACTORY( MuonPrepRawDataCollectionProviderTool )
DECLARE_TOOL_FACTORY( MuonPRDSelectionTool )

DECLARE_FACTORY_ENTRIES( MuonPrepRawDataProviderTools ) 
{
  DECLARE_TOOL( MuonLayerHashProviderTool )
  DECLARE_TOOL( MuonPrepRawDataCollectionProviderTool )
  DECLARE_TOOL( MuonPRDSelectionTool )
}
