// $Id: IsolationSelection_entries.cxx 678002 2015-06-24 15:39:36Z morrisj $

// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "IsolationSelection/IsolationSelectionTool.h"
#include "../TestIsolationAthenaAlg.h"


DECLARE_NAMESPACE_TOOL_FACTORY( CP , IsolationSelectionTool )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( CP , TestIsolationAthenaAlg )

DECLARE_FACTORY_ENTRIES( IsolationSelection ) {
   DECLARE_NAMESPACE_TOOL( CP , IsolationSelectionTool )
   DECLARE_NAMESPACE_ALGORITHM( CP , TestIsolationAthenaAlg )
}
