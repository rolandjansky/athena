#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../METHypo.h"
#include "../TEHypo.h"
#include "../XSHypo.h"

DECLARE_ALGORITHM_FACTORY( HLT::MET::METHypo )
DECLARE_ALGORITHM_FACTORY( HLT::MET::TEHypo )
DECLARE_ALGORITHM_FACTORY( HLT::MET::XSHypo )

DECLARE_FACTORY_ENTRIES( TrigMissingETHypo ) {
  DECLARE_ALGORITHM( HLT::MET::METHypo )
  DECLARE_ALGORITHM( HLT::MET::TEHypo )
  DECLARE_ALGORITHM( HLT::MET::XSHypo )
}

