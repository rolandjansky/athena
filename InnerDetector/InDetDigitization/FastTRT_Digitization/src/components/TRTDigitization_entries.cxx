#include "../../FastTRT_Digitization/TRTFastDigitization.h"
#include "../../FastTRT_Digitization/TRTFastDigitizationTool.h"
#include "../../FastTRT_Digitization/ITRTFastDigitizationTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( TRTFastDigitization )

DECLARE_TOOL_FACTORY(TRTFastDigitizationTool)

DECLARE_FACTORY_ENTRIES( FastTRT_Digitization ) {
  DECLARE_ALGORITHM( TRTFastDigitization )
  DECLARE_ALGTOOL( TRTFastDigitizationTool )
}

