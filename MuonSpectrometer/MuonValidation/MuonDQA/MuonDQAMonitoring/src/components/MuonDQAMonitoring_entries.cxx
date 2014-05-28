#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MuonDQAMonitoring/MuonDQAEvent.h"
#include "MuonDQAMonitoring/MuonDQAEventInfo.h"
#include "MuonDQAMonitoring/MuonEventInfoMonTool.h" 

using namespace MuonDQA;
 
DECLARE_ALGORITHM(MuonDQAEvent)
DECLARE_ALGORITHM(MuonDQAEventInfo)
DECLARE_TOOL_FACTORY(MuonEventInfoMonTool) 
   
DECLARE_FACTORY_ENTRIES(MuonDQAMonitoring)
{
   DECLARE_ALGTOOL(MuonEventInfoMonTool) 
} 
 
