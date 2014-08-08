

#include "TBMonitoring/TBBPCMonTool.h"
#include "TBMonitoring/TBMWPCMonTool.h"
#include "TBMonitoring/TBPhaseMonTool.h"
#include "TBMonitoring/TBScintillatorMonTool.h"
#include "TBMonitoring/TBTailCatcherRawMonTool.h"
#include "TBMonitoring/TBTriggerPatternUnitMonTool.h"
#include "TBMonitoring/TBTDCRawMonTool.h"
#include "TBMonitoring/TBADCRawMonTool.h"
#include "TBMonitoring/TBCaloResponseMonTool.h"
#include "TBMonitoring/TBBeamQualityDoubleHitsMonTool.h"

#include "GaudiKernel/DeclareFactoryEntries.h"
 

DECLARE_TOOL_FACTORY(TBBPCMonTool)
DECLARE_TOOL_FACTORY(TBMWPCMonTool)
DECLARE_TOOL_FACTORY(TBPhaseMonTool)
DECLARE_TOOL_FACTORY(TBScintillatorMonTool)
DECLARE_TOOL_FACTORY(TBTailCatcherRawMonTool)
DECLARE_TOOL_FACTORY(TBTriggerPatternUnitMonTool)
DECLARE_TOOL_FACTORY(TBTDCRawMonTool)
DECLARE_TOOL_FACTORY(TBADCRawMonTool)
DECLARE_TOOL_FACTORY(TBCaloResponseMonTool)
DECLARE_TOOL_FACTORY(TBBeamQualityDoubleHitsMonTool)

DECLARE_FACTORY_ENTRIES(TBMonitoring) {
  DECLARE_ALGTOOL(TBBPCMonTool)
  DECLARE_ALGTOOL(TBMWPCMonTool)
  DECLARE_ALGTOOL(TBPhaseMonTool)
  DECLARE_ALGTOOL(TBScintillatorMonTool)
  DECLARE_ALGTOOL(TBTailCatcherRawMonTool)
  DECLARE_ALGTOOL(TBTriggerPatternUnitMonTool)
  DECLARE_ALGTOOL(TBTDCRawMonTool)
  DECLARE_ALGTOOL(TBADCRawMonTool)
  DECLARE_ALGTOOL(TBCaloResponseMonTool)
  DECLARE_ALGTOOL(TBBeamQualityDoubleHitsMonTool)
}
