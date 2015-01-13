
#include "MuonAGDD/MuonAGDDTool.h"
#include "MuonAGDD/NSWAGDDTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY(MuonAGDDTool)
DECLARE_TOOL_FACTORY(NSWAGDDTool)

DECLARE_FACTORY_ENTRIES(MuonAGDD) {
	DECLARE_ALGTOOL	  ( MuonAGDDTool )
	DECLARE_ALGTOOL	  ( NSWAGDDTool )
}

