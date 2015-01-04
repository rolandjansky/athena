#include "GaudiKernel/DeclareFactoryEntries.h"
#include "CoolLumiUtilities/FillParamsTool.h"
#include "CoolLumiUtilities/BunchGroupTool.h"
#include "CoolLumiUtilities/BunchLumisTool.h"
#include "CoolLumiUtilities/OnlineLumiCalibrationTool.h"

DECLARE_TOOL_FACTORY( FillParamsTool )
DECLARE_TOOL_FACTORY( BunchGroupTool )
DECLARE_TOOL_FACTORY( BunchLumisTool )
DECLARE_TOOL_FACTORY( OnlineLumiCalibrationTool )

DECLARE_FACTORY_ENTRIES( CoolLumiUtilities ) {
  DECLARE_ALGTOOL( FillParamsTool )
  DECLARE_ALGTOOL( BunchGroupTool )
  DECLARE_ALGTOOL( BunchLumisTool )
  DECLARE_ALGTOOL( OnlineLumiCalibrationTool )
}
