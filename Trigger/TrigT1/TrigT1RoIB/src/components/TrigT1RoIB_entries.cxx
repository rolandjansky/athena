// $Id: TrigT1RoIB_entries.cxx 500598 2012-05-14 15:39:58Z krasznaa $

// Gaudi include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "../RoIBuilder.h"
#include "../RoIBTester.h"
#include "../RecRoIBTester.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY( ROIB, RoIBuilder )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( ROIB, RoIBTester )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( ROIB, RecRoIBTester )

DECLARE_FACTORY_ENTRIES( TrigT1RoIB ) {
    DECLARE_NAMESPACE_ALGORITHM( ROIB, RoIBuilder )
    DECLARE_NAMESPACE_ALGORITHM( ROIB, RoIBTester )
    DECLARE_NAMESPACE_ALGORITHM( ROIB, RecRoIBTester )
}
