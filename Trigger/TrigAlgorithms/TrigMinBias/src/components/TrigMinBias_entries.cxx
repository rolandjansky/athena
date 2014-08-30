#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrigMinBias/TrigTrackCounter.h"
#include "TrigMinBias/TrigTrackCounterHypo.h"
#include "TrigMinBias/TrigVertexCounter.h"
#include "TrigMinBias/TrigVertexCounterHypo.h"

DECLARE_ALGORITHM_FACTORY( TrigTrackCounter )
DECLARE_ALGORITHM_FACTORY( TrigTrackCounterHypo )
DECLARE_ALGORITHM_FACTORY( TrigVertexCounter )
DECLARE_ALGORITHM_FACTORY( TrigVertexCounterHypo )


DECLARE_FACTORY_ENTRIES(  TrigMinBias  ) {

DECLARE_ALGORITHM( TrigTrackCounter )
DECLARE_ALGORITHM( TrigTrackCounterHypo )
DECLARE_ALGORITHM( TrigVertexCounter )
DECLARE_ALGORITHM( TrigVertexCounterHypo )

}
