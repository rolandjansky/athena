#include "InDetGlobalMonitoring/InDetGlobalManager.h"
#include "InDetGlobalMonitoring/InDetGlobalTopBottomMonTool.h"
#include "InDetGlobalMonitoring/InDetGlobalHitsMonTool.h"
#include "InDetGlobalMonitoring/InDetGlobalMotherMonTool.h"
#include "InDetGlobalMonitoring/InDetGlobalSynchMonTool.h"
#include "InDetGlobalMonitoring/InDetGlobalNoiseOccupancyMonTool.h"
#include "InDetGlobalMonitoring/InDetGlobalTrackMonTool.h"
#include "InDetGlobalMonitoring/InDetGlobalBCMTool.h"
#include "InDetGlobalMonitoring/InDetGlobalPixelTool.h"
#include "InDetGlobalMonitoring/InDetGlobalBeamSpotMonTool.h"
#include "InDetGlobalMonitoring/InDetGlobalPrimaryVertexMonTool.h"
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
}

