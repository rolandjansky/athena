#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "MM_Digitization/MM_DigitizationTool.h"
#include "MM_Digitization/MM_Digitizer.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

DECLARE_ALGORITHM_FACTORY( MM_Digitizer )

DECLARE_TOOL_FACTORY( MM_DigitizationTool )

DECLARE_FACTORY_ENTRIES(MM_Digitization) {
    DECLARE_ALGORITHM( MM_Digitizer )
    DECLARE_TOOL( MM_Response_DigitTool )
    DECLARE_TOOL( MM_DigitizationTool )
}
