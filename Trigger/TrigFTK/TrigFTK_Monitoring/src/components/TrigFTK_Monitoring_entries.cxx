
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrigFTK_Monitoring/FtkHltEfficiencyFex.h"

using namespace TrigFTK;

DECLARE_ALGORITHM_FACTORY( FtkHltEfficiencyFex)

DECLARE_FACTORY_ENTRIES( TrigFTK_Monitoring ) 
{
  DECLARE_ALGORITHM( FtkHltEfficiencyFex );
}
