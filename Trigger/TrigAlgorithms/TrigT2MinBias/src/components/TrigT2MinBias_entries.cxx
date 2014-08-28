#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrigT2MinBias/T2MbtsFex.h"
#include "TrigT2MinBias/T2MbtsHypo.h"
#include "TrigT2MinBias/TrigCountSpacePoints.h"
#include "TrigT2MinBias/TrigCountSpacePointsHypo.h"
#include "TrigT2MinBias/TrigCountTrtHits.h"
#include "TrigT2MinBias/TrigCountTrtHitsHypo.h"
#include "TrigT2MinBias/T2ZdcFex.h"
#include "TrigT2MinBias/T2ZdcHypo.h"

DECLARE_ALGORITHM_FACTORY(T2MbtsFex)
DECLARE_ALGORITHM_FACTORY(T2MbtsHypo)
DECLARE_ALGORITHM_FACTORY(TrigCountSpacePoints)
DECLARE_ALGORITHM_FACTORY(TrigCountSpacePointsHypo)
DECLARE_ALGORITHM_FACTORY(TrigCountTrtHits)
DECLARE_ALGORITHM_FACTORY(TrigCountTrtHitsHypo)
DECLARE_ALGORITHM_FACTORY(T2ZdcFex)
DECLARE_ALGORITHM_FACTORY(T2ZdcHypo)

DECLARE_FACTORY_ENTRIES(TrigT2MinBias) {
  DECLARE_ALGORITHM(T2MbtsFex)
  DECLARE_ALGORITHM(T2MbtsHypo)
  DECLARE_ALGORITHM(TrigCountSpacePoints)
  DECLARE_ALGORITHM(TrigCountSpacePointsHypo)
  DECLARE_ALGORITHM(TrigCountTrtHits)
  DECLARE_ALGORITHM(TrigCountTrtHitsHypo)
  DECLARE_ALGORITHM(T2ZdcFex)
  DECLARE_ALGORITHM(T2ZdcHypo)
}
