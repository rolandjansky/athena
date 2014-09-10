#include "GaudiKernel/DeclareFactoryEntries.h"

//#include "TrigBphysMonitoring/BCompositeParticleBuilder.h"
//#include "TrigBphysMonitoring/HLTBphysMonTool.h"
#include "TrigBphysMonitoring/HLTXAODBphysMonTool.h"

using namespace BPhys;

//DECLARE_TOOL_FACTORY( HLTBphysMonTool )
DECLARE_TOOL_FACTORY( HLTXAODBphysMonTool )
//DECLARE_TOOL_FACTORY( BCompositeParticleBuilder )

DECLARE_FACTORY_ENTRIES(TrigBphysMonitoring) {
  //DECLARE_ALGTOOL( HLTBphysMonTool )
  DECLARE_ALGTOOL( HLTXAODBphysMonTool )
  //DECLARE_TOOL( BCompositeParticleBuilder )
}
