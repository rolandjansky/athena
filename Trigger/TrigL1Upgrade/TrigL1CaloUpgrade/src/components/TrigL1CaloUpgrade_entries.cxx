#include "src/TrigT1CaloEFex.h"
#include "src/TrigT1CaloforwEFex.h"
#include "src/TrigT1CaloTauFex.h"
#include "src/SimpleSuperCellChecks.h"
#include "src/SimpleLArDigitsChecks.h"
#include "src/LArFex.h"
#include "src/SuperCellBCIDAlg.h"
#include "src/TrigT1CaloRun3TauFex.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( TrigT1CaloEFex )
DECLARE_ALGORITHM_FACTORY( TrigT1CaloforwEFex )
DECLARE_ALGORITHM_FACTORY( TrigT1CaloTauFex )
DECLARE_ALGORITHM_FACTORY( SimpleSuperCellChecks )
DECLARE_ALGORITHM_FACTORY( SimpleLArDigitsChecks )
DECLARE_ALGORITHM_FACTORY( LArFex )
DECLARE_ALGORITHM_FACTORY( SuperCellBCIDAlg )
DECLARE_ALGORITHM_FACTORY( TrigT1CaloRun3TauFex )
DECLARE_FACTORY_ENTRIES(TrigL1CaloUpgrade) {
	DECLARE_ALGORITHM( TrigT1CaloEFex );
	DECLARE_ALGORITHM( TrigT1CaloforwEFex );
	DECLARE_ALGORITHM( SimpleSuperCellChecks );
	DECLARE_ALGORITHM( SimpleLArDigitsChecks );
	DECLARE_ALGORITHM( TrigT1CaloTauFex );
	DECLARE_ALGORITHM( LArFex );
	DECLARE_ALGORITHM( SuperCellBCIDAlg );
	DECLARE_ALGORITHM( TrigT1CaloRun3TauFex );
}
