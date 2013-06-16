// $Id: TrigT1Muctpi_entries.cxx 500504 2012-05-14 11:39:28Z krasznaa $

// Gaudi include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "../Algorithms/L1Muctpi.h"
#include "../Algorithms/L1MuctpiPatGen.h"
#include "../Algorithms/L1MuctpiTestReadResults.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY( LVL1MUCTPI, L1Muctpi )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( LVL1MUCTPI, L1MuctpiPatGen )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( LVL1MUCTPI, L1MuctpiTestReadResults )

DECLARE_FACTORY_ENTRIES( TrigT1Muctpi ) {
   DECLARE_NAMESPACE_ALGORITHM( LVL1MUCTPI, L1Muctpi )
   DECLARE_NAMESPACE_ALGORITHM( LVL1MUCTPI, L1MuctpiPatGen )
   DECLARE_NAMESPACE_ALGORITHM( LVL1MUCTPI, L1MuctpiTestReadResults )
}
