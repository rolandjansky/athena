#include "GaudiKernel/DeclareFactoryEntries.h"

#include "egammaRec/egammaBuilder.h"
#include "egammaRec/topoEgammaBuilder.h"
#include "egammaRec/egammaForwardBuilder.h"
#include "egammaRec/egammaTruthAssociationAlg.h"
#include "egammaRec/egammaTrackSlimmer.h"
#include "egammaRec/egammaLockCollections.h"


DECLARE_ALGORITHM_FACTORY( egammaBuilder            )
DECLARE_ALGORITHM_FACTORY( topoEgammaBuilder        )
DECLARE_ALGORITHM_FACTORY( egammaForwardBuilder     )
DECLARE_ALGORITHM_FACTORY( egammaTruthAssociationAlg)
DECLARE_ALGORITHM_FACTORY( egammaTrackSlimmer       )
DECLARE_ALGORITHM_FACTORY( egammaLockCollections        )


DECLARE_FACTORY_ENTRIES(egammaRec) {
     DECLARE_ALGORITHM( egammaBuilder             )
     DECLARE_ALGORITHM( topoEgammaBuilder         )
     DECLARE_ALGORITHM( egammaForwardBuilder      )
     DECLARE_ALGORITHM( egammaTruthAssociationAlg )
     DECLARE_ALGORITHM( egammaTrackSlimmer        )
     DECLARE_ALGORITHM( egammaLockCollections     )

}
