#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrigTRTHTHCounter.h"
#include "TrigTRTHTHhypo.h"


DECLARE_ALGORITHM_FACTORY( TrigTRTHTHCounter )
DECLARE_ALGORITHM_FACTORY( TrigTRTHTHhypo )

DECLARE_FACTORY_ENTRIES( TrigTRTHighTHitCounter )
{
    DECLARE_ALGORITHM( TrigTRTHTHCounter )
    DECLARE_ALGORITHM( TrigTRTHTHhypo )
}

