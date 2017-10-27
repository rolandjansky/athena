#include "../LArTBH6TriggerTimeTool.h"
#include "../LArTBH6BeamInfo.h"
#include "../LArGeoH62004EventActionTool.h"
#include "../LArGeoH62004SteppingActionTool.h"
#include "../RadLenNtupleTool.h"

// New SDs
#include "../H62004EMECSDTool.h"
#include "../H62004FCALSDTool.h"
#include "../H62004HECSDTool.h"
#include "../H62004ActiveSDTool.h"
#include "../H62004DeadSDTool.h"
#include "../H62004InactiveSDTool.h"

// SDs
#include "../LArG4H62004ActiveSDTool.h"
#include "../LArG4H62004DeadSDTool.h"
#include "../LArG4H62004EMECSDTool.h"
#include "../LArG4H62004FCALSDTool.h"
#include "../LArG4H62004HECSDTool.h"
#include "../LArG4H62004InactiveSDTool.h"
#include "../LArG4H6BeamSDTool.h"
#include "../LArG4H6WarmTCSDTool.h"
#include "../LArG4TBECBeamChSDTool.h"
#include "../TBECBackOuterBarretteTool.h"
#include "../TBECInnerModuleTool.h"
#include "../TBECOuterModuleTool.h"

// Calculator Services
#include "../LArFCALH62004CalibCalculatorBase.h"
#include "../LArG4H6COLDTCMod0Calculator.h"
#include "../LArG4H6WarmTCCalculator.h"
#include "../LArG4H62004DeadCalibrationCalculator.h"
#include "../LArG4H6COLDTCMod0CalibCalculator.h"


// UA tools
DECLARE_TOOL_FACTORY( LArTBH6TriggerTimeTool )
DECLARE_TOOL_FACTORY( G4UA::LArGeoH62004EventActionTool )
DECLARE_TOOL_FACTORY( G4UA::LArGeoH62004SteppingActionTool )
DECLARE_TOOL_FACTORY( G4UA::RadLenNtupleTool )

// SD tools
DECLARE_TOOL_FACTORY( LArG4::H62004EMECSDTool )
DECLARE_TOOL_FACTORY( LArG4::H62004FCALSDTool )
DECLARE_TOOL_FACTORY( LArG4::H62004HECSDTool )
DECLARE_TOOL_FACTORY( LArG4::H62004ActiveSDTool )
DECLARE_TOOL_FACTORY( LArG4::H62004DeadSDTool )
DECLARE_TOOL_FACTORY( LArG4::H62004InactiveSDTool )

DECLARE_TOOL_FACTORY( LArG4H62004ActiveSDTool )
DECLARE_TOOL_FACTORY( LArG4H62004DeadSDTool )
DECLARE_TOOL_FACTORY( LArG4H62004EMECSDTool )
DECLARE_TOOL_FACTORY( LArG4H62004FCALSDTool )
DECLARE_TOOL_FACTORY( LArG4H62004HECSDTool )
DECLARE_TOOL_FACTORY( LArG4H62004InactiveSDTool )
DECLARE_TOOL_FACTORY( LArG4H6BeamSDTool )
DECLARE_TOOL_FACTORY( LArG4H6WarmTCSDTool )
DECLARE_TOOL_FACTORY( LArG4TBECBeamChSDTool )
DECLARE_TOOL_FACTORY( TBECBackOuterBarretteTool )
DECLARE_TOOL_FACTORY( TBECInnerModuleTool )
DECLARE_TOOL_FACTORY( TBECOuterModuleTool )

DECLARE_ALGORITHM_FACTORY(LArTBH6BeamInfo)

//Calculators

DECLARE_SERVICE_FACTORY(LArFCALH62004CalibCalculatorBase)
DECLARE_SERVICE_FACTORY(LArG4H6COLDTCMod0Calculator)
DECLARE_SERVICE_FACTORY(LArG4H6WarmTCCalculator)
DECLARE_SERVICE_FACTORY(LArG4H62004DeadCalibrationCalculator)
DECLARE_SERVICE_FACTORY(LArG4H6COLDTCMod0CalibCalculator)

