#include "GaudiKernel/DeclareFactoryEntries.h"
#include "HGTD_TimeCalibrationTools/StraightLineTOFcorrectionTool.h"

using namespace HGTD;

DECLARE_TOOL_FACTORY(StraightLineTOFcorrectionTool)

DECLARE_FACTORY_ENTRIES( HGTD_TimeCalibrationTools )
{
  DECLARE_TOOL( StraightLineTOFcorrectionTool )
}
