#include "JetCalibTools/JetCalibrationTool.h"

#include "JetCalibTools/CalibrationMethods/JetPileupCorrection.h"
#include "JetCalibTools/CalibrationMethods/ResidualOffsetCorrection.h"
#include "JetCalibTools/CalibrationMethods/EtaJESCorrection.h"
#include "JetCalibTools/CalibrationMethods/InsituDataCorrection.h"

DECLARE_TOOL_FACTORY( JetCalibrationTool )
DECLARE_TOOL_FACTORY( JetPileupCorrection )
DECLARE_TOOL_FACTORY( ResidualOffsetCorrection )
DECLARE_TOOL_FACTORY( EtaJESCorrection )
DECLARE_TOOL_FACTORY( InsituDataCorrection )

