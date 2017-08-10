
#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../MyPackageAlg.h"

DECLARE_ALGORITHM_FACTORY( MyPackageAlg )


#include "AthAsgExUnittest/MyPackageTool.h"
DECLARE_TOOL_FACTORY( MyPackageTool )

DECLARE_FACTORY_ENTRIES( AthAsgExUnittest ) 
{
  DECLARE_TOOL( MyPackageTool );
  DECLARE_ALGORITHM( MyPackageAlg );
}
