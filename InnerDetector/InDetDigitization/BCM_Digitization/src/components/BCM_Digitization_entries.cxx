#include "../BCM_Digitization.h"
#include "../BCM_DigitizationTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( BCM_Digitization )
DECLARE_TOOL_FACTORY( BCM_DigitizationTool )

DECLARE_FACTORY_ENTRIES( BCM_Digitization ) {
    DECLARE_ALGORITHM( BCM_Digitization )
    DECLARE_ALGTOOL( BCM_DigitizationTool )
}

