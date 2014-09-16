#include "GaudiKernel/DeclareFactoryEntries.h"

#include "egammaRec/egammaBuilder.h"
#include "egammaRec/egammaForwardBuilder.h"
#include "egammaRec/egammaTruthAssociationAlg.h"
#include "egammaRec/egammaTrackSlimmer.h"


DECLARE_ALGORITHM_FACTORY( egammaBuilder             )
DECLARE_ALGORITHM_FACTORY( egammaForwardBuilder      )
DECLARE_ALGORITHM_FACTORY( egammaTruthAssociationAlg )
DECLARE_ALGORITHM_FACTORY( egammaTrackSlimmer       )



DECLARE_FACTORY_ENTRIES(egammaRec) {
     DECLARE_ALGORITHM( egammaBuilder            )
     DECLARE_ALGORITHM( egammaForwardBuilder     )
     DECLARE_ALGORITHM( egammaTruthAssociationAlg )
     DECLARE_ALGORITHM( egammaTrackSlimmer       )

}
