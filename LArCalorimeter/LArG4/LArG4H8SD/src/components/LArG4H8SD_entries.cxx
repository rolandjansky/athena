#include "../LArG4H8CalibSDTool.h"
#include "../H8CalibSDTool.h"

#include "../LArTBCryostatCalibrationCalculator.h"
#include "../H8CalibrationDefaultCalculator.h"

DECLARE_TOOL_FACTORY( LArG4::H8CalibSDTool )
DECLARE_TOOL_FACTORY( LArG4H8CalibSDTool )

DECLARE_SERVICE_FACTORY(LArTBCryostatCalibrationCalculator)
DECLARE_SERVICE_FACTORY(H8CalibrationDefaultCalculator)

