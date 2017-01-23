// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "InDetTruthVertexValidation/InDetVertexTruthMatchTool.h"
#include "../InDetVertexTruthMatchAlgorithm.h"

DECLARE_TOOL_FACTORY( InDetVertexTruthMatchTool )

DECLARE_ALGORITHM_FACTORY( InDetVertexTruthMatchAlgorithm )

DECLARE_FACTORY_ENTRIES( InDetTruthVertexValidation ) {
  
  DECLARE_TOOL( InDetVertexTruthMatchTool )
    
  DECLARE_ALGORITHM( InDetVertexTruthMatchAlgorithm )

}
