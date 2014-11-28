#include "../egammaIso.h"
#include "../egammaShowerShape.h"
#include "../egammaEnergyPositionAllSamples.h"
#include "../egammaPreSamplerShape.h"
#include "../egammaStripsShape.h"
#include "../egammaMiddleShape.h"
#include "../egammaBackShape.h"
#include "../egammaqweta1c.h"
#include "../egammaqweta2c.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY     ( egammaShowerShape   )
DECLARE_TOOL_FACTORY     ( egammaEnergyPositionAllSamples )
DECLARE_TOOL_FACTORY     ( egammaPreSamplerShape )
DECLARE_TOOL_FACTORY     ( egammaStripsShape )
DECLARE_TOOL_FACTORY     ( egammaMiddleShape )
DECLARE_TOOL_FACTORY     ( egammaBackShape )
DECLARE_TOOL_FACTORY     ( egammaIso        )
DECLARE_TOOL_FACTORY     ( egammaqweta1c    )
DECLARE_TOOL_FACTORY     ( egammaqweta2c    )


DECLARE_FACTORY_ENTRIES(egammaCaloTools) {
    DECLARE_TOOL     ( egammaIso       )  
    DECLARE_TOOL     ( egammaShowerShape  )  
    DECLARE_TOOL     ( egammaEnergyPositionAllSamples )
    DECLARE_TOOL     ( egammaPreSamplerShape )
    DECLARE_TOOL     ( egammaStripsShape )
    DECLARE_TOOL     ( egammaMiddleShape )
    DECLARE_TOOL     ( egammaBackShape )
    DECLARE_TOOL     ( egammaqweta1c   )  
    DECLARE_TOOL     ( egammaqweta2c   )  

}
 
