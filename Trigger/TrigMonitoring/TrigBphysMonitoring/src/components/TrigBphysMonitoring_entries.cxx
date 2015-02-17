#include "GaudiKernel/DeclareFactoryEntries.h"

#include "TrigBphysMonitoring/HLTXAODBphysMonTool.h"

//#include "TrigBphysMonitoring/IBphysTrigDiMuDecoratorTool.h"
#include "../BphysTrigDiMuDecoratorTool.h"
#include "../BphysTrigDecoratorAlg.h"

DECLARE_TOOL_FACTORY( HLTXAODBphysMonTool )
//DECLARE_TOOL_FACTORY( IBphysTrigDiMuDecoratorTool )
DECLARE_TOOL_FACTORY( BphysTrigDiMuDecoratorTool )
DECLARE_ALGORITHM_FACTORY( BphysTrigDecoratorAlg )

DECLARE_FACTORY_ENTRIES(TrigBphysMonitoring) {
    DECLARE_ALGTOOL( HLTXAODBphysMonTool )
    //DECLARE_ALGTOOL( IBphysTrigDiMuDecoratorTool )
    DECLARE_ALGTOOL( BphysTrigDiMuDecoratorTool )
    DECLARE_ALGORITHM( BphysTrigDecoratorAlg )
}
