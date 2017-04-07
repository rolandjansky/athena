#include "src/TrigT1CaloEFex.h"
#include "src/TrigT1CaloTauFex.h"
#include "src/SimpleSuperCellChecks.h"
#include "src/EFexAnalysis.h"
#include "src/SimpleLArDigitsChecks.h"
#include "src/DumpAll.h"
#include "src/LArFex.h"
#include "src/LArFexAnalysis.h"
#include "src/CaloBandwidth.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( TrigT1CaloEFex )
DECLARE_ALGORITHM_FACTORY( TrigT1CaloTauFex )
DECLARE_ALGORITHM_FACTORY( SimpleSuperCellChecks )
DECLARE_ALGORITHM_FACTORY( SimpleLArDigitsChecks )
DECLARE_ALGORITHM_FACTORY( EFexAnalysis )
DECLARE_ALGORITHM_FACTORY( DumpAll )
DECLARE_ALGORITHM_FACTORY( LArFex )
DECLARE_ALGORITHM_FACTORY( LArFexAnalysis )
DECLARE_ALGORITHM_FACTORY( CaloBandwidth )
DECLARE_FACTORY_ENTRIES(TrigL1CaloUpgrade) {
	DECLARE_ALGORITHM( TrigT1CaloEFex );
	DECLARE_ALGORITHM( SimpleSuperCellChecks );
	DECLARE_ALGORITHM( SimpleLArDigitsChecks );
	DECLARE_ALGORITHM( EFexAnalysis );
	DECLARE_ALGORITHM( TrigT1CaloTauFex );
	DECLARE_ALGORITHM( DumpAll );
	DECLARE_ALGORITHM( LArFex );
	DECLARE_ALGORITHM( LArFexAnalysis );
	DECLARE_ALGORITHM( CaloBandwidth );
}
