#include "MuonTagTools/CombinedMuonTagTool.h"
#include "MuonTagTools/MuonTagTool.h"

#include "GaudiKernel/DeclareFactoryEntries.h"
 
//DECLARE_TOOL_FACTORY( CombinedMuonTagTool )
DECLARE_TOOL_FACTORY( MuonTagTool )
 
DECLARE_FACTORY_ENTRIES( MuonTagTools ) {
  // DECLARE_TOOL( CombinedMuonTagTool )
  DECLARE_TOOL( MuonTagTool )
}




