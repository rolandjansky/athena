#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"




using namespace Muon;

DECLARE_TOOL_FACTORY( MuonIdHelperTool )

DECLARE_FACTORY_ENTRIES( MuonIdHelpers ) 
{
  DECLARE_TOOL( MuonIdHelperTool );
}
