#include "GaudiKernel/DeclareFactoryEntries.h"
#include "CaloCondBlobAlgs/CaloNoiseDbExample.h"
#include "CaloCondBlobAlgs/CaloCoolIdTool.h"

DECLARE_ALGORITHM_FACTORY( CaloNoiseDbExample )
DECLARE_TOOL_FACTORY( CaloCoolIdTool )
  
DECLARE_FACTORY_ENTRIES(CaloTools) {
  DECLARE_ALGORITHM( CaloNoiseDbExample )
  DECLARE_TOOL( CaloCoolIdTool )
}

