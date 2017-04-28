#include "GaudiKernel/DeclareFactoryEntries.h"

#include "JetTileCorrection/JetTileCorrectionTool.h"
#include "../JetTileCorrectionAlg.h"

DECLARE_NAMESPACE_TOOL_FACTORY( CP, JetTileCorrectionTool )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( CP, JetTileCorrectionAlg )

DECLARE_FACTORY_ENTRIES( JetTileCorrection ) 
{
  DECLARE_NAMESPACE_TOOL( CP, JetTileCorrectionTool );
  DECLARE_NAMESPACE_ALGORITHM( CP, JetTileCorrectionAlg );
}
