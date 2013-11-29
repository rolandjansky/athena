#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../MuonTestEDM.h"
#include "../IdHelpersValidation.h"

DECLARE_ALGORITHM_FACTORY( MuonTestEDM )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( Muon, IdHelpersValidation )

DECLARE_FACTORY_ENTRIES( MuonTestEDM )
{
	DECLARE_ALGORITHM( MuonTestEDM )
	DECLARE_NAMESPACE_ALGORITHM( Muon, IdHelpersValidation )
}




