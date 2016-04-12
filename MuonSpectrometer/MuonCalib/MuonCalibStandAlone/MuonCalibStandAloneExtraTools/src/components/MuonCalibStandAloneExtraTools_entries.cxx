#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MuonCalibStandAloneExtraTools/CalibExtraNtupleLoader.h"
#include "MuonCalibStandAloneExtraTools/ExtraRegionSelector.h"
#include "MuonCalibStandAloneExtraTools/ExtraRegionSelectorSuperFast.h"
#include "MuonCalibStandAloneExtraTools/ExtraRegionSelectorFast.h"
#include "MuonCalibStandAloneExtraTools/TrackSecondCoordinateToSegment.h"
#include "MuonCalibStandAloneExtraTools/NtupleMdtDqaTool.h"
#include "../NtupleSplitterTool.h"

using namespace MuonCalib;

DECLARE_TOOL_FACTORY (CalibExtraNtupleLoader)
DECLARE_TOOL_FACTORY (ExtraRegionSelector)
DECLARE_TOOL_FACTORY (ExtraRegionSelectorFast)
DECLARE_TOOL_FACTORY (ExtraRegionSelectorSuperFast)
DECLARE_TOOL_FACTORY (TrackSecondCoordinateToSegment)
DECLARE_TOOL_FACTORY (NtupleMdtDqaTool)
DECLARE_TOOL_FACTORY (NtupleSplitterTool)

DECLARE_FACTORY_ENTRIES(MuonCalibStandAloneExtraTools) {
  DECLARE_TOOL (CalibExtraNtupleLoader)
  DECLARE_TOOL (ExtraRegionSelector)
  DECLARE_TOOL (ExtraRegionSelectorFast)
  DECLARE_TOOL (ExtraRegionSelectorSuperFast)
  DECLARE_TOOL (TrackSecondCoordinateToSegment)
  DECLARE_TOOL (NtupleMdtDqaTool)
  DECLARE_TOOL (NtupleSplitterTool)
}

