#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../L1CaloDecoder.h"
#include "../FakeRoI.h"
#include "../RoIGraph.h"
#include "../FakeCTP.h"

DECLARE_ALGORITHM_FACTORY(L1CaloDecoder)
DECLARE_ALGORITHM_FACTORY(FakeRoI)
DECLARE_ALGORITHM_FACTORY(RoIGraph)
DECLARE_ALGORITHM_FACTORY(FakeCTP)
DECLARE_TOOL_FACTORY(MonitoredTool)

DECLARE_FACTORY_ENTRIES( L1Decoder )
{
    DECLARE_ALGORITHM(L1CaloDecoder)
    DECLARE_ALGORITHM(FakeRoI)
	DECLARE_ALGORITHM(RoIGraph)
    DECLARE_ALGORITHM(FakeCTP)
    DECLARE_TOOL(MonitoredTool)
}
