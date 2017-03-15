#include "GaudiKernel/DeclareFactoryEntries.h"
#include "egammaAlgs/egammaBuilder.h"
#include "egammaAlgs/topoEgammaBuilder.h"
#include "egammaAlgs/egammaForwardBuilder.h"
#include "egammaAlgs/egammaTruthAssociationAlg.h"
#include "egammaAlgs/egammaTrackSlimmer.h"
#include "egammaAlgs/egammaFinalizeClusters.h"
#include "egammaAlgs/egammaLockCollections.h"
#include "egammaAlgs/EMBremCollectionBuilder.h"
#include "egammaAlgs/EMVertexBuilder.h"


DECLARE_ALGORITHM_FACTORY( egammaBuilder            )
DECLARE_ALGORITHM_FACTORY( topoEgammaBuilder        )
DECLARE_ALGORITHM_FACTORY( egammaForwardBuilder     )
DECLARE_ALGORITHM_FACTORY( egammaTruthAssociationAlg)
DECLARE_ALGORITHM_FACTORY( egammaTrackSlimmer       )
DECLARE_ALGORITHM_FACTORY( egammaFinalizeClusters   )
DECLARE_ALGORITHM_FACTORY( egammaLockCollections    )
DECLARE_ALGORITHM_FACTORY( EMBremCollectionBuilder  )
DECLARE_ALGORITHM_FACTORY( EMVertexBuilder  )


DECLARE_FACTORY_ENTRIES(egammaAlgs) {
     DECLARE_ALGORITHM( egammaBuilder             )
     DECLARE_ALGORITHM( topoEgammaBuilder         )
     DECLARE_ALGORITHM( egammaForwardBuilder      )
     DECLARE_ALGORITHM( egammaTruthAssociationAlg )
     DECLARE_ALGORITHM( egammaTrackSlimmer        )
     DECLARE_ALGORITHM( egammaFinalizeClusters    )
     DECLARE_ALGORITHM( egammaLockCollections     )
     DECLARE_ALGORITHM( EMBremCollectionBuilder   )
     DECLARE_ALGORITHM( EMVertexBuilder   )

}
