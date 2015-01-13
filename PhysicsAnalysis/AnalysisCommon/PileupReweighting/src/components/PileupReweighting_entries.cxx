#include "GaudiKernel/DeclareFactoryEntries.h"


#include "PileupReweighting/PileupReweightingTool.h"

#include "../PileupReweightingProvider.h"

DECLARE_NAMESPACE_TOOL_FACTORY( CP, PileupReweightingTool )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( CP, PileupReweightingProvider )

DECLARE_FACTORY_ENTRIES( PileupReweighting ) 
{
   DECLARE_NAMESPACE_TOOL( CP, PileupReweightingTool )
    DECLARE_NAMESPACE_ALGORITHM( CP, PileupReweightingProvider )
}


