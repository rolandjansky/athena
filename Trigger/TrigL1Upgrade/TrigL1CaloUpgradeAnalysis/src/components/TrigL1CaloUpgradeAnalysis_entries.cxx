#include "src/EFexAnalysis.h"
#include "src/DumpAll.h"
#include "src/LArFexAnalysis.h"
#include "src/CaloBandwidth.h"
#include "src/EFexEratioAlgo.h"
#include "src/EFexEratioRateAlgo.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( EFexAnalysis )
DECLARE_ALGORITHM_FACTORY( DumpAll )
DECLARE_ALGORITHM_FACTORY( LArFexAnalysis )
DECLARE_ALGORITHM_FACTORY( CaloBandwidth )
DECLARE_ALGORITHM_FACTORY( EFexEratioAlgo )
DECLARE_ALGORITHM_FACTORY( EFexEratioRateAlgo )
DECLARE_FACTORY_ENTRIES(TrigL1CaloUpgradeAnalysis) {
	DECLARE_ALGORITHM( EFexAnalysis );
	DECLARE_ALGORITHM( DumpAll );
	DECLARE_ALGORITHM( LArFexAnalysis );
	DECLARE_ALGORITHM( CaloBandwidth );
	DECLARE_ALGORITHM( EFexEratioAlgo );
	DECLARE_ALGORITHM( EFexEratioRateAlgo );
}
