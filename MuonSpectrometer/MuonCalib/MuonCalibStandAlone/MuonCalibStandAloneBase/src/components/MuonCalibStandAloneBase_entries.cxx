#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MuonCalibStandAloneBase/RegionSelectionSvc.h"
#include "MuonCalibStandAloneBase/CalibrationDummyIOTool.h"
#include "MuonCalibStandAloneBase/CalibrationTeeIOTool.h"

using namespace MuonCalib;

DECLARE_SERVICE_FACTORY ( RegionSelectionSvc )
DECLARE_TOOL_FACTORY ( CalibrationDummyIOTool )
DECLARE_TOOL_FACTORY ( CalibrationTeeIOTool )

DECLARE_FACTORY_ENTRIES(MuonCalibStandAloneBase) {
  DECLARE_SERVICE(RegionSelectionSvc)
  DECLARE_TOOL(CalibrationDummyIOTool)
  DECLARE_TOOL(CalibrationTeeIOTool)
}

