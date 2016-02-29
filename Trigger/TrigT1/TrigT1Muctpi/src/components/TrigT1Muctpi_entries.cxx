// $Id: TrigT1Muctpi_entries.cxx 726107 2016-02-25 11:04:42Z wengler $

// Gaudi include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "../Algorithms/L1Muctpi.h"
#include "../Algorithms/L1MuctpiPatGen.h"
#include "../Algorithms/L1MuctpiTestReadResults.h"
#include "../TrigT1Muctpi/L1MuctpiTool.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY( LVL1MUCTPI, L1Muctpi )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( LVL1MUCTPI, L1MuctpiPatGen )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( LVL1MUCTPI, L1MuctpiTestReadResults )

DECLARE_NAMESPACE_TOOL_FACTORY( LVL1MUCTPI, L1MuctpiTool )


DECLARE_FACTORY_ENTRIES( TrigT1Muctpi ) {
   DECLARE_NAMESPACE_ALGORITHM( LVL1MUCTPI, L1Muctpi )
   DECLARE_NAMESPACE_ALGORITHM( LVL1MUCTPI, L1MuctpiPatGen )
   DECLARE_NAMESPACE_ALGORITHM( LVL1MUCTPI, L1MuctpiTestReadResults )

   DECLARE_NAMESPACE_TOOL( LVL1MUCTPI, L1MuctpiTool )
}
