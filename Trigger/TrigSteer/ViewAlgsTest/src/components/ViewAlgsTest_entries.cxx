
#include "GaudiKernel/DeclareFactoryEntries.h"


#include "../TestFEXAlg.h"
#include "../TestHypoAlg.h"
#include "../TestHypoTool.h"




DECLARE_ALGORITHM_FACTORY( TestFEXAlg )
DECLARE_ALGORITHM_FACTORY( TestHypoAlg )
DECLARE_TOOL_FACTORY( TestHypoTool )

DECLARE_FACTORY_ENTRIES( ViewAlgsTest )
{
  DECLARE_ALGORITHM( TestFEXAlg )
  DECLARE_ALGORITHM( TestHypoAlg )
  DECLARE_TOOL( TestHypoTool )
}
