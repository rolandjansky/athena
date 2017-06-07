#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../PadTdsOfflineTool.h"
#include "../PadTriggerLogicOfflineTool.h"
#include "../StripTdsOfflineTool.h"
#include "../StripClusterTool.h"
#include "../StripSegmentTool.h"
#include "../MMStripTdsOfflineTool.h"
#include "../MMFPGAOfflineTool.h"
#include "../MMTriggerTool.h"

using NSWL1::PadTdsOfflineTool;
using NSWL1::PadTriggerLogicOfflineTool;
using NSWL1::StripTdsOfflineTool;

using NSWL1::StripClusterTool;
using NSWL1::StripSegmentTool;
using NSWL1::MMStripTdsOfflineTool;
using NSWL1::MMFPGAOfflineTool;
using NSWL1::MMTriggerTool;

DECLARE_TOOL_FACTORY( PadTdsOfflineTool )
DECLARE_TOOL_FACTORY( PadTriggerLogicOfflineTool )
DECLARE_TOOL_FACTORY( StripTdsOfflineTool )
DECLARE_TOOL_FACTORY( StripClusterTool )
DECLARE_TOOL_FACTORY( StripSegmentTool )
DECLARE_TOOL_FACTORY( MMStripTdsOfflineTool )
DECLARE_TOOL_FACTORY( MMFPGAOfflineTool )
DECLARE_TOOL_FACTORY( MMTriggerTool )

DECLARE_FACTORY_ENTRIES ( TrigT1NSWSimTool ) {
  DECLARE_TOOL ( PadTdsOfflineTool )
  DECLARE_TOOL ( PadTriggerLogicOfflineTool )
  DECLARE_TOOL ( StripTdsOfflineTool )
  DECLARE_TOOL ( StripClusterTool )
  DECLARE_TOOL ( StripSegmentTool )
  DECLARE_TOOL ( MMStripTdsOfflineTool )
  DECLARE_TOOL ( MMFPGAOfflineTool )
  DECLARE_TOOL ( MMTriggerTool )
}
