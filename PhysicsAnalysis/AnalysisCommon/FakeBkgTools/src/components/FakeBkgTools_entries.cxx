#include "GaudiKernel/DeclareFactoryEntries.h"

#include "FakeBkgTools/ApplyFakeFactor.h"
#include "FakeBkgTools/AsymptMatrixTool.h"
#include "FakeBkgTools/LhoodMM_tools.h"

DECLARE_TOOL_FACTORY( CP::ApplyFakeFactor )
DECLARE_TOOL_FACTORY( CP::AsymptMatrixTool )
DECLARE_TOOL_FACTORY( CP::LhoodMM_tools )

DECLARE_FACTORY_ENTRIES( FakeBkgTools ) 
{
  DECLARE_TOOL( CP::ApplyFakeFactor );
  DECLARE_TOOL( CP::AsymptMatrixTool );
  DECLARE_TOOL( CP::LhoodMM_tools );
  
}