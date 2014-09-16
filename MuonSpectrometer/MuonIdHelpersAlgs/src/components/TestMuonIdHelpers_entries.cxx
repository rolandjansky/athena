#include "MuonIdHelpersAlgs/TestMuonIdHelpers.h"
#include "MuonIdHelpersAlgs/CscHashId.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( TestMuonIdHelpers )
DECLARE_ALGORITHM_FACTORY( CscHashId )

DECLARE_FACTORY_ENTRIES(MuonIdHelpersAlgs) {
    DECLARE_ALGORITHM( TestMuonIdHelpers )
    DECLARE_ALGORITHM( CscHashId )
}

