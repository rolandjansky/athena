//#include "TrigMissingETHypo/TrigL2MissingETHypo.h"
#include "TrigMissingETHypo/TrigEFMissingETHypo.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( TrigEFMissingETHypo )
  //DECLARE_ALGORITHM_FACTORY( TrigL2MissingETHypo )

DECLARE_FACTORY_ENTRIES( TrigMissingETHypo ) {
  DECLARE_ALGORITHM( TrigEFMissingETHypo )
    //  DECLARE_ALGORITHM( TrigL2MissingETHypo )
}

