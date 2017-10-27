// MT-friendly LAr SD tools
#include "../EMBSDTool.h"
#include "../EMECSDTool.h"
#include "../HECSDTool.h"
#include "../FCALSDTool.h"
#include "../MiniFCALSDTool.h"
#include "../ActiveSDTool.h"
#include "../InactiveSDTool.h"
#include "../DeadSDTool.h"

#include "../CalibrationDefaultCalculator.h"

DECLARE_TOOL_FACTORY( LArG4::EMBSDTool )
DECLARE_TOOL_FACTORY( LArG4::EMECSDTool )
DECLARE_TOOL_FACTORY( LArG4::HECSDTool )
DECLARE_TOOL_FACTORY( LArG4::FCALSDTool )
DECLARE_TOOL_FACTORY( LArG4::MiniFCALSDTool )
DECLARE_TOOL_FACTORY( LArG4::ActiveSDTool )
DECLARE_TOOL_FACTORY( LArG4::InactiveSDTool )
DECLARE_TOOL_FACTORY( LArG4::DeadSDTool )

DECLARE_SERVICE_FACTORY( LArG4::CalibrationDefaultCalculator )

