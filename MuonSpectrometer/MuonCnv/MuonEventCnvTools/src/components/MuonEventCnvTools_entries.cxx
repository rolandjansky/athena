
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MuonEventCnvTools/MuonEventCnvTool.h"


DECLARE_NAMESPACE_TOOL_FACTORY( Muon, MuonEventCnvTool )

DECLARE_FACTORY_ENTRIES( MuonEventCnvTools )
{
	DECLARE_NAMESPACE_TOOL( Muon, MuonEventCnvTool )
}




