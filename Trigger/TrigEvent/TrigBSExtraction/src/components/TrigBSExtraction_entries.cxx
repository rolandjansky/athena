
#include "TrigBSExtraction/TrigBSExtraction.h"
#include "TrigBSExtraction/TrigHLTtoxAODConversion.h"
#include "TrigBSExtraction/TrigBStoxAODTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( TrigBSExtraction )
DECLARE_ALGORITHM_FACTORY( TrigHLTtoxAODConversion )
DECLARE_TOOL_FACTORY( TrigBStoxAODTool )

DECLARE_FACTORY_ENTRIES( TrigBSExtraction ) {
  DECLARE_ALGORITHM( TrigBSExtraction )
  DECLARE_ALGORITHM( TrigHLTtoxAODConversion )
  DECLARE_TOOL( TrigBStoxAODTool )
}
