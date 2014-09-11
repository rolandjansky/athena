#include "../AthRetrySequencer.h"
#include "../AthSequencer.h"
#include "../AthAnalysisSequencer.h"
#include "../AthEventCounter.h"
#include "../AthPrescaler.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( AthRetrySequencer )
DECLARE_ALGORITHM_FACTORY( AthSequencer )
DECLARE_ALGORITHM_FACTORY( AthAnalysisSequencer )
DECLARE_ALGORITHM_FACTORY( AthEventCounter )
DECLARE_ALGORITHM_FACTORY( AthPrescaler )


DECLARE_FACTORY_ENTRIES(GaudiSequencer) {
  DECLARE_ALGORITHM( AthRetrySequencer )
  DECLARE_ALGORITHM( AthSequencer )
  DECLARE_ALGORITHM( AthAnalysisSequencer )
  DECLARE_ALGORITHM( AthEventCounter )
  DECLARE_ALGORITHM( AthPrescaler )
}


