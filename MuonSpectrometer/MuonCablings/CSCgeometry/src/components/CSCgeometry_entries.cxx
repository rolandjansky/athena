#include "GaudiKernel/DeclareFactoryEntries.h"
#include "CSCgeometry/CSCgeometrySvc.h"
#include "CSCgeometry/CSCgeometryTester.h"

DECLARE_ALGORITHM_FACTORY( CSCgeometryTester )
DECLARE_SERVICE_FACTORY( CSCgeometrySvc )

DECLARE_FACTORY_ENTRIES( CSCgeometry ){
    DECLARE_ALGORITHM( CSCgeometryTester )
    DECLARE_SERVICE( CSCgeometrySvc )
}
