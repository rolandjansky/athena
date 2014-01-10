#include "TGC_Digitization/TGCDigitizer.h"
#include "TGC_Digitization/TgcDigitizationTool.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( TGCDigitizer )

DECLARE_TOOL_FACTORY( TgcDigitizationTool )

DECLARE_FACTORY_ENTRIES(TGC_Digitization) {
    DECLARE_ALGORITHM( TGCDigitizer )
    
    DECLARE_TOOL( TgcDigitizationTool )
}
