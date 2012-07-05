#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MdtCalibIOSvc/MdtCalibInputSvc.h"
#include "MdtCalibIOSvc/MdtCalibOutputDbSvc.h"
#include "MdtCalibIOSvc/CalibrationFileIOTool.h"
#include "MdtCalibIOSvc/CalibrationOracleFileIOTool.h"

using namespace MuonCalib;

DECLARE_SERVICE_FACTORY ( MdtCalibOutputDbSvc )
DECLARE_SERVICE_FACTORY ( MdtCalibInputSvc )
DECLARE_TOOL_FACTORY ( CalibrationFileIOTool )
DECLARE_TOOL_FACTORY ( CalibrationOracleFileIOTool )

DECLARE_FACTORY_ENTRIES(MdtCalibIOSvc) {
  DECLARE_SERVICE(MdtCalibOutputDbSvc)
  DECLARE_SERVICE(MdtCalibInputSvc)
  DECLARE_TOOL(CalibrationFileIOTool)
  DECLARE_TOOL(CalibrationOracleFileIOTool)
}

