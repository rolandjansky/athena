#include "GaudiKernel/DeclareFactoryEntries.h"
#include "SCT_Cabling/SCT_CablingSvc.h"
#include "../SCT_FillCablingFromText.h"
#include "../SCT_FillCablingFromCoraCool.h"
#include "SCT_Cabling/SCT_TestCablingAlg.h"

DECLARE_SERVICE_FACTORY(SCT_CablingSvc)
DECLARE_SERVICE_FACTORY(SCT_FillCablingFromText)
DECLARE_SERVICE_FACTORY(SCT_FillCablingFromCoraCool)
DECLARE_ALGORITHM_FACTORY(SCT_TestCablingAlg)

DECLARE_FACTORY_ENTRIES(SCT_Cabling) {
  DECLARE_SERVICE( SCT_CablingSvc)
  DECLARE_SERVICE( SCT_FillCablingFromText)
  DECLARE_SERVICE( SCT_FillCablingFromCoraCool)
  DECLARE_ALGORITHM( SCT_TestCablingAlg )
}

