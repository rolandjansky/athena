#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TopEDM_Test/TopEDM_WriteTest.h"
#include "TopEDM_Test/TopEDM_ReadTest.h"
#include "TopEDM_Test/TopTemplateExample.h"

DECLARE_ALGORITHM_FACTORY( TopEDM_WriteTest )
DECLARE_ALGORITHM_FACTORY( TopEDM_ReadTest )
DECLARE_ALGORITHM_FACTORY( TopTemplateExample )
  
DECLARE_FACTORY_ENTRIES( TopEDM_Test ) {
  DECLARE_ALGORITHM( TopEDM_WriteTest )
  DECLARE_ALGORITHM( TopEDM_ReadTest )
  DECLARE_ALGORITHM( TopTemplateExample )
}
