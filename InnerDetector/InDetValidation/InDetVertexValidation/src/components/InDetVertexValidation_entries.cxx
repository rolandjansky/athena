#include "GaudiKernel/DeclareFactoryEntries.h"
#include "InDetVertexValidation/InDetVertexValidationTool.h"
#include "../VertexAnalysisAlgorithm.h"

#ifndef XAOD_ANALYSIS
DECLARE_TOOL_FACTORY(InDetVertexValidationTool)
#endif

#ifdef XAOD_ANALYSIS
DECLARE_ALGORITHM_FACTORY(VertexAnalysisAlgorithm)
#endif

DECLARE_FACTORY_ENTRIES(InDetVertexValidation){
#ifndef XAOD_ANALYSIS
  DECLARE_TOOL(InDetVertexValidationTool)
#endif
    
#ifdef XAOD_ANALYSIS
    DECLARE_ALGORITHM(VertexAnalysisAlgorithm)
#endif
    
    }
