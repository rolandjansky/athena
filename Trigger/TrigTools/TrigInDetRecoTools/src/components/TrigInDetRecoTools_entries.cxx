#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetRecoTools/TrigL2PattRecoStrategyA.h"
#include "TrigInDetRecoTools/TrigL2PattRecoStrategyB.h"
#include "TrigInDetRecoTools/TrigL2PattRecoStrategyC.h"
#include "TrigInDetRecoTools/TrigL2PattRecoStrategyF.h"
#include "TrigInDetRecoTools/TrigL2PattRecoStrategyT.h"
#include "TrigInDetRecoTools/TrigL2DupTrackRemovalTool.h"
#include "TrigInDetRecoTools/TrigInDetRoadMakerTool.h"
#include "TrigInDetRecoTools/FTK_DataProviderTool.h"
#include "../TrigL2LayerSetPredictorTool.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY(TrigL2PattRecoStrategyA)
DECLARE_TOOL_FACTORY(TrigL2PattRecoStrategyB)
DECLARE_TOOL_FACTORY(TrigL2PattRecoStrategyC)
DECLARE_TOOL_FACTORY(TrigL2PattRecoStrategyF)
DECLARE_TOOL_FACTORY(TrigL2PattRecoStrategyT)
DECLARE_TOOL_FACTORY(TrigL2DupTrackRemovalTool)
DECLARE_TOOL_FACTORY(TrigInDetRoadMakerTool)
DECLARE_TOOL_FACTORY(FTK_DataProviderTool)
DECLARE_TOOL_FACTORY(TrigL2LayerSetPredictorTool)

DECLARE_FACTORY_ENTRIES(TrigInDetRecoTools)
{
  DECLARE_TOOL(TrigL2PattRecoStrategyA)
  DECLARE_TOOL(TrigL2PattRecoStrategyB)
  DECLARE_TOOL(TrigL2PattRecoStrategyC)
  DECLARE_TOOL(TrigL2PattRecoStrategyF)
  DECLARE_TOOL(TrigL2PattRecoStrategyT)
  DECLARE_TOOL(TrigL2DupTrackRemovalTool)
  DECLARE_TOOL(TrigInDetRoadMakerTool)
  DECLARE_TOOL(FTK_DataProviderTool)
  DECLARE_TOOL(TrigL2LayerSetPredictorTool)
}
