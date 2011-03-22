#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MuonPrdSelector/MuonPrdSelectorAlg.h"
#include "MuonPrdSelector/MuonIdCutTool.h"

DECLARE_ALGORITHM_FACTORY( MuonPrdSelectorAlg )
DECLARE_TOOL_FACTORY( MuonIdCutTool )

DECLARE_FACTORY_ENTRIES( MuonPrdSelector ) {
  DECLARE_ALGORITHM( MuonPrdSelectorAlg )
  DECLARE_TOOL( MuonIdCutTool )
}


