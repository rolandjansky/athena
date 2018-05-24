#include "../InDetGlobalManager.h"
#include "../InDetGlobalTopBottomMonTool.h"
#include "../InDetGlobalHitsMonTool.h"
#include "../InDetGlobalMotherMonTool.h"
#include "../InDetGlobalSynchMonTool.h"
#include "../InDetGlobalNoiseOccupancyMonTool.h"
#include "../InDetGlobalTrackMonTool.h"
#include "../InDetGlobalBCMTool.h"
#include "../InDetGlobalPixelTool.h"
#include "../InDetGlobalBeamSpotMonTool.h"
#include "../InDetGlobalPrimaryVertexMonTool.h"
#include "../InDetGlobalErrorMonTool.h"
#include "../InDetGlobalBackgroundMonTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY(InDetGlobalManager)
DECLARE_TOOL_FACTORY(InDetGlobalHitsMonTool)
DECLARE_TOOL_FACTORY(InDetGlobalMotherMonTool)
DECLARE_TOOL_FACTORY(InDetGlobalSynchMonTool)
DECLARE_TOOL_FACTORY(InDetGlobalNoiseOccupancyMonTool)
DECLARE_TOOL_FACTORY(InDetGlobalTrackMonTool)
DECLARE_TOOL_FACTORY(InDetGlobalBCMTool)
DECLARE_TOOL_FACTORY(InDetGlobalPixelTool)
DECLARE_TOOL_FACTORY(InDetGlobalBeamSpotMonTool)
DECLARE_TOOL_FACTORY(InDetGlobalPrimaryVertexMonTool)
DECLARE_TOOL_FACTORY(InDetGlobalErrorMonTool)
DECLARE_TOOL_FACTORY(InDetGlobalBackgroundMonTool)



DECLARE_FACTORY_ENTRIES(InDetGlobalMonitoring) {
  DECLARE_ALGORITHM(InDetGlobalManager);
  DECLARE_ALGTOOL(InDetGlobalHitsMonTool);
  DECLARE_ALGTOOL(InDetGlobalMotherMonTool);
  DECLARE_ALGTOOL(InDetGlobalSynchMonTool);
  DECLARE_ALGTOOL(InDetGlobalNoiseOccupancyMonTool);
  DECLARE_ALGTOOL(InDetGlobalTrackMonTool);
  DECLARE_ALGTOOL(InDetGlobalBCMTool);
  DECLARE_ALGTOOL(InDetGlobalPixelTool);
  DECLARE_ALGTOOL(InDetGlobalBeamSpotMonTool);
  DECLARE_ALGTOOL(InDetGlobalPrimaryVertexMonTool);
  DECLARE_ALGTOOL(InDetGlobalErrorMonTool);
  DECLARE_ALGTOOL(InDetGlobalBackgroundMonTool);
}

