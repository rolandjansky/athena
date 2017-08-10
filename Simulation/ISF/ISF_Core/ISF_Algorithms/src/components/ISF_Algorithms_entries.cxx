#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../SimKernel.h"
#include "../CollectionMerger.h"
#include "../SimHitTreeCreator.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY( ISF , SimKernel        )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( ISF , CollectionMerger )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( ISF , SimHitTreeCreator)
