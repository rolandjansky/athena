// $Id: FsrUtils_entries.cxx  2014-07-18 tcuhadar@cern.ch $

// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "FsrUtils/FsrPhotonTool.h"
#include "IsolationSelection/IIsolationSelectionTool.h"
#include "../ToolTester.h"


DECLARE_NAMESPACE_TOOL_FACTORY( FSR, FsrPhotonTool)

DECLARE_NAMESPACE_ALGORITHM_FACTORY( FSR, ToolTester )

DECLARE_FACTORY_ENTRIES( FsrUtils  ) {

   DECLARE_NAMESPACE_TOOL( FSR, FsrPhotonTool )

   DECLARE_NAMESPACE_ALGORITHM( FSR, ToolTester )

}
