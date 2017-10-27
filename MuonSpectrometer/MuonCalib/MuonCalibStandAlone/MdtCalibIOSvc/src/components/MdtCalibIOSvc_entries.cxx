#include "MdtCalibIOSvc/MdtCalibInputSvc.h"
#include "MdtCalibIOSvc/MdtCalibOutputDbSvc.h"
#include "MdtCalibIOSvc/CalibrationFileIOTool.h"
#include "MdtCalibIOSvc/CalibrationOracleFileIOTool.h"

using namespace MuonCalib;

DECLARE_SERVICE_FACTORY ( MdtCalibOutputDbSvc )
DECLARE_SERVICE_FACTORY ( MdtCalibInputSvc )
DECLARE_TOOL_FACTORY ( CalibrationFileIOTool )
DECLARE_TOOL_FACTORY ( CalibrationOracleFileIOTool )

