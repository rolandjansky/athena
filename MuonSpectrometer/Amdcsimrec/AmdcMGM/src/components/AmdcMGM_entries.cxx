#include "GaudiKernel/DeclareFactoryEntries.h"
#include "AmdcMGM/AmdcDumpGeoModel.h"

DECLARE_ALGORITHM_FACTORY( AmdcDumpGeoModel )

DECLARE_FACTORY_ENTRIES( AmdcMGM ) {
	DECLARE_ALGORITHM( AmdcDumpGeoModel );
}

