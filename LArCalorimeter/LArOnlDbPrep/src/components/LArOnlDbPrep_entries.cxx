#include "GaudiKernel/DeclareFactoryEntries.h"
#include "LArOnlDbPrep/LArDSPThresholdFillInline.h"
#include "LArOnlDbPrep/LArGeoWeightsFill.h"

DECLARE_ALGORITHM_FACTORY ( LArDSPThresholdFillInline )
DECLARE_ALGORITHM_FACTORY ( LArGeoWeightsFill )

DECLARE_FACTORY_ENTRIES(LArOnlDbPrep) {
  DECLARE_ALGORITHM ( LArDSPThresholdFillInline )
  DECLARE_ALGORITHM ( LArGeoWeightsFill )
}
