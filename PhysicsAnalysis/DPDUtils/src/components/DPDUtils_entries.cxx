#include "GaudiKernel/DeclareFactoryEntries.h"
#include "DPDUtils/ZeeFilter.h"
#include "DPDUtils/DPDPhotonFilter.h"
#include "DPDUtils/DPDTagTool.h"
#include "DPDUtils/ObjectThinner.h"

DECLARE_ALGORITHM_FACTORY( DPDPhotonFilter )
DECLARE_ALGORITHM_FACTORY( ZeeFilter )
DECLARE_ALGORITHM_FACTORY( ObjectThinner )

DECLARE_TOOL_FACTORY( DPDTagTool )

DECLARE_FACTORY_ENTRIES( DPDUtils ) {

  DECLARE_ALGORITHM(DPDPhotonFilter)
  DECLARE_ALGORITHM(ZeeFilter)
  DECLARE_ALGORITHM( ObjectThinner )

  DECLARE_TOOL( DPDTagTool )

}
