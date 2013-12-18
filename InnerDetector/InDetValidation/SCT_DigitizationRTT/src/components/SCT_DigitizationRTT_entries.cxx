#include "../SCT_DigitizationRTT.h"
#include "../SCT_ReadoutModeChecker.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( SCT_DigitizationRTT )
  DECLARE_ALGORITHM_FACTORY( SCT_ReadoutModeChecker )

DECLARE_FACTORY_ENTRIES(SCT_DigitizationRTT) {
    DECLARE_ALGORITHM( SCT_DigitizationRTT );
    DECLARE_ALGORITHM( SCT_ReadoutModeChecker );
}
