#include "../TRTDigitization.h"
#include "../TRTDigitizationTool.h"
#include "../TRT_SimDriftTimeTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( TRTDigitization )

DECLARE_TOOL_FACTORY(TRTDigitizationTool)
DECLARE_TOOL_FACTORY( TRT_SimDriftTimeTool )

DECLARE_FACTORY_ENTRIES( TRT_Digitization ) {
  DECLARE_ALGORITHM( TRTDigitization )
  DECLARE_ALGTOOL( TRTDigitizationTool )
  DECLARE_ALGTOOL( TRT_SimDriftTimeTool )
}

