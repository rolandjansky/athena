#include "CavernBkgGenerator/GenerateCavernBkg.h"
#include "CavernBkgGenerator/MuonBackgroundConverter.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( GenerateCavernBkg )
DECLARE_ALGORITHM_FACTORY( MuonBackgroundConverter )

DECLARE_FACTORY_ENTRIES( CavernBkgGenerator ) {
    DECLARE_ALGORITHM( GenerateCavernBkg )
    DECLARE_ALGORITHM( MuonBackgroundConverter  )
}
