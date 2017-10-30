#include "MuonCalibStandAloneExtraTools/CalibExtraNtupleLoader.h"
#include "MuonCalibStandAloneExtraTools/ExtraRegionSelector.h"
#include "MuonCalibStandAloneExtraTools/ExtraRegionSelectorSuperFast.h"
#include "MuonCalibStandAloneExtraTools/ExtraRegionSelectorFast.h"
#include "MuonCalibStandAloneExtraTools/TrackSecondCoordinateToSegment.h"
#include "MuonCalibStandAloneExtraTools/NtupleMdtDqaTool.h"
#include "../NtupleSplitterTool.h"

using namespace MuonCalib;

DECLARE_COMPONENT( CalibExtraNtupleLoader )
DECLARE_COMPONENT( ExtraRegionSelector )
DECLARE_COMPONENT( ExtraRegionSelectorFast )
DECLARE_COMPONENT( ExtraRegionSelectorSuperFast )
DECLARE_COMPONENT( TrackSecondCoordinateToSegment )
DECLARE_COMPONENT( NtupleMdtDqaTool )
DECLARE_COMPONENT( NtupleSplitterTool )

