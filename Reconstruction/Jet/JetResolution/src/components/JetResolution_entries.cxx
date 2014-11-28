// Gaudi/Athena includes
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local includes
#include "JetResolution/JERTool.h"
#include "JetResolution/JERSmearingTool.h"
#include "../JERTestAlg.h"

DECLARE_TOOL_FACTORY( JERTool )
DECLARE_TOOL_FACTORY( JERSmearingTool )
DECLARE_ALGORITHM_FACTORY( JERTestAlg )

DECLARE_FACTORY_ENTRIES( JetResolution ) {

  DECLARE_TOOL( JERTool )
  DECLARE_TOOL( JERSmearingTool )
  DECLARE_ALGORITHM( JERTestAlg )

}
