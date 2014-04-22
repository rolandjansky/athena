#include "TrigTRT_TrackExtensionTool/TrigTRT_TrackExtensionTool.h"
#include "TrigTRT_TrackExtensionTool/TrigTRT_DetElementRoadTool.h"
#include "TrigTRT_TrackExtensionTool/TrigMagneticFieldTool.h"
#include "TrigTRT_TrackExtensionTool/TrigTRT_CombinedExtensionTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY(TrigTRT_TrackExtensionTool)
DECLARE_TOOL_FACTORY(TrigTRT_DetElementRoadTool)
DECLARE_TOOL_FACTORY(TrigMagneticFieldTool)
DECLARE_TOOL_FACTORY(TrigTRT_CombinedExtensionTool)

DECLARE_FACTORY_ENTRIES(TrigTRT_TrackExtensionTool)
{
  DECLARE_TOOL(TrigTRT_TrackExtensionTool);
  DECLARE_TOOL(TrigTRT_DetElementRoadTool);
  DECLARE_TOOL(TrigMagneticFieldTool);
  DECLARE_TOOL(TrigTRT_CombinedExtensionTool);
}

