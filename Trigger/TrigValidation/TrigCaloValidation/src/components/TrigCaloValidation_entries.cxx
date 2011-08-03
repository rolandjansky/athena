
#include "TrigCaloValidation/TrigCaloValidation.h"
#include "TrigCaloValidation/TrigCaloClusterValidation.h"
#include "TrigCaloValidation/TrigL2CaloValidation.h"
#include "TrigCaloValidation/TrigFEBValidation.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( TrigCaloValidation )
DECLARE_ALGORITHM_FACTORY( TrigCaloClusterValidation )
DECLARE_ALGORITHM_FACTORY( TrigL2CaloValidation )
DECLARE_ALGORITHM_FACTORY( TrigFEBValidation )
DECLARE_FACTORY_ENTRIES (TrigCaloValidation) {
	DECLARE_ALGORITHM( TrigCaloValidation );
	DECLARE_ALGORITHM( TrigCaloClusterValidation );
	DECLARE_ALGORITHM( TrigL2CaloValidation );
	DECLARE_ALGORITHM( TrigFEBValidation );
}
