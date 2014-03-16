#include "TrigExMTHistNtup/MTHist.h"
#include "TrigExMTHistNtup/MTNtup.h"
#include "TrigExMTHistNtup/MTTHist.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( MTHist )
DECLARE_ALGORITHM_FACTORY( MTNtup )
DECLARE_ALGORITHM_FACTORY( MTTHist )

DECLARE_FACTORY_ENTRIES(TrigExMTHistNtup) {
    DECLARE_ALGORITHM( MTHist );
    DECLARE_ALGORITHM( MTNtup );
    DECLARE_ALGORITHM( MTTHist );
}
