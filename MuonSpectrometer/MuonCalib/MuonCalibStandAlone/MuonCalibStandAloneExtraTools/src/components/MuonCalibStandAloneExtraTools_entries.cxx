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

