#include "GoodRunsListsUser/DummyDumperAlg.h"
#include "GoodRunsListsUser/TriggerSelectorAlg.h"
#include "GoodRunsListsUser/GoodRunsListSelectorAlg.h"
#include "GoodRunsListsUser/GRLTriggerSelectorAlg.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( DummyDumperAlg )
DECLARE_ALGORITHM_FACTORY( TriggerSelectorAlg )
DECLARE_ALGORITHM_FACTORY( GoodRunsListSelectorAlg )
DECLARE_ALGORITHM_FACTORY( GRLTriggerSelectorAlg )

DECLARE_FACTORY_ENTRIES( GoodRunsListsUser ) {
    DECLARE_ALGORITHM( DummyDumperAlg )
    DECLARE_ALGORITHM( TriggerSelectorAlg )
    DECLARE_ALGORITHM( GoodRunsListSelectorAlg )
    DECLARE_ALGORITHM( GRLTriggerSelectorAlg )
}

