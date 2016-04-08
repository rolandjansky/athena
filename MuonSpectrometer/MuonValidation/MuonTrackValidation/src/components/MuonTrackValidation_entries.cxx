#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../MuonStationNtupleHelperTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY( Muon, MuonStationNtupleHelperTool )

DECLARE_FACTORY_ENTRIES( MuonTrackValidation )
{
	DECLARE_NAMESPACE_TOOL( Muon, MuonStationNtupleHelperTool )
}

