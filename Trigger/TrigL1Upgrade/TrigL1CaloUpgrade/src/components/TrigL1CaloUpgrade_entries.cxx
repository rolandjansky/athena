#include "src/TrigT1CaloEFex.h"
#include "src/TrigT1CaloTauFex.h"
#include "src/SimpleSuperCellChecks.h"
#include "src/EFexAnalysis.h"
#include "src/SimpleLArDigitsChecks.h"
#include "src/DumpAll.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( TrigT1CaloEFex )
DECLARE_ALGORITHM_FACTORY( TrigT1CaloTauFex )
DECLARE_ALGORITHM_FACTORY( SimpleSuperCellChecks )
DECLARE_ALGORITHM_FACTORY( SimpleLArDigitsChecks )
DECLARE_ALGORITHM_FACTORY( EFexAnalysis )
DECLARE_ALGORITHM_FACTORY( DumpAll )
DECLARE_FACTORY_ENTRIES(TrigL1CaloUpgrade) {
	DECLARE_ALGORITHM( TrigT1CaloEFex );
	DECLARE_ALGORITHM( SimpleSuperCellChecks );
	DECLARE_ALGORITHM( SimpleLArDigitsChecks );
	DECLARE_ALGORITHM( EFexAnalysis );
	DECLARE_ALGORITHM( TrigT1CaloTauFex );
	DECLARE_ALGORITHM( DumpAll );
}
