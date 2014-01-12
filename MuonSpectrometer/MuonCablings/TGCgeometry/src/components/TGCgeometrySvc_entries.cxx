#include "TGCgeometry/TGCgeometrySvc.h"
#include "TGCgeometry/TGCgeometryTester.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( TGCgeometryTester )
DECLARE_SERVICE_FACTORY( TGCgeometrySvc )

DECLARE_FACTORY_ENTRIES(TGCgeometry) {
    DECLARE_ALGORITHM( TGCgeometryTester )
    DECLARE_SERVICE( TGCgeometrySvc )
}

