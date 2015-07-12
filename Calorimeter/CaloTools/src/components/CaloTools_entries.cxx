#include "CaloTools/CaloCompactCellTool.h"
#include "CaloTools/CaloNoiseTool.h"
#include "CaloTools/CaloMBAverageTool.h"
#include "CaloTools/CaloNoiseToolDB.h"
#include "CaloTools/SimpleNoiseTool.h"
#include "CaloTools/SimpleNoiseToolFromTextFile.h"
#include "CaloTools/CaloAffectedTool.h"
#include "CaloTools/CaloLumiBCIDTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY( CaloCompactCellTool )
DECLARE_TOOL_FACTORY( CaloNoiseTool )
DECLARE_TOOL_FACTORY( CaloMBAverageTool )
DECLARE_TOOL_FACTORY( CaloNoiseToolDB )
DECLARE_TOOL_FACTORY( SimpleNoiseTool )
DECLARE_TOOL_FACTORY( SimpleNoiseToolFromTextFile )
DECLARE_TOOL_FACTORY( CaloAffectedTool )
DECLARE_TOOL_FACTORY( CaloLumiBCIDTool )

DECLARE_FACTORY_ENTRIES(CaloTools) {
    DECLARE_TOOL( CaloCompactCellTool )
    DECLARE_TOOL( CaloNoiseTool )
    DECLARE_TOOL( CaloMBAverageTool )
    DECLARE_TOOL( CaloNoiseToolDB )
    DECLARE_TOOL( SimpleNoiseTool )
    DECLARE_TOOL( SimpleNoiseToolFromTextFile )
    DECLARE_TOOL( CaloAffectedTool )
    DECLARE_TOOL( CaloLumiBCIDTool )
}
