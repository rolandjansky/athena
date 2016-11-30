
#include "TrigCaloHypo/TrigEFCaloHypoNoise.h"
#include "TrigCaloHypo/TrigL2JetHypo.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( TrigEFCaloHypoNoise )
DECLARE_ALGORITHM_FACTORY( TrigL2JetHypo )

DECLARE_FACTORY_ENTRIES( TrigCaloHypo ) {

   DECLARE_ALGORITHM( TrigEFCaloHypoNoise )
   DECLARE_ALGORITHM( TrigL2JetHypo )

}


