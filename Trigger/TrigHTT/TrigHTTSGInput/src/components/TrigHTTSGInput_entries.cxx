#include "GaudiKernel/DeclareFactoryEntries.h"

#include "TrigHTTSGInput/HTT_SGToRawHitsTool.h"
#include "TrigHTTSGInput/HTTRawHitsWrapperAlg.h"

DECLARE_COMPONENT( HTT_SGToRawHitsTool )
DECLARE_COMPONENT( HTTRawHitsWrapperAlg )
DECLARE_FACTORY_ENTRIES( TrigHTTSGInput )
{
  DECLARE_ALGORITHM( HTTRawHitsWrapperAlg );
}

