#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../SimKernel.h"
#include "../SimEventFilter.h"
#include "../CollectionMerger.h"
#include "../RenameHitCollectionsAlg.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY( ISF , SimKernel        )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( ISF , SimEventFilter   )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( ISF , CollectionMerger )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( ISF , RenameHitCollectionsAlg )
