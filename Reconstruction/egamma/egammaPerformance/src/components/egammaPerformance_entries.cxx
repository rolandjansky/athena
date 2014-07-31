#include "egammaPerformance/egammaMonTool.h"
#include "egammaPerformance/photonMonTool.h"
#include "egammaPerformance/forwardEgammaMonTool.h"
#include "egammaPerformance/physicsMonTool.h"
#include "egammaPerformance/IEgammaMonTool.h"
#include "egammaPerformance/ephysicsMonTool.h"
#include "egammaPerformance/TopphysicsMonTool.h"
#include "egammaPerformance/electronMonTool.h"
#include "egammaPerformance/softEMonTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY     ( egammaMonTool            )
DECLARE_TOOL_FACTORY     ( photonMonTool            )
DECLARE_TOOL_FACTORY     ( electronMonTool          )
DECLARE_TOOL_FACTORY     ( forwardEgammaMonTool     )
DECLARE_TOOL_FACTORY     ( physicsMonTool           )
DECLARE_TOOL_FACTORY     ( ephysicsMonTool          )
DECLARE_TOOL_FACTORY     ( TopphysicsMonTool        )
DECLARE_TOOL_FACTORY     ( softEMonTool             )
DECLARE_FACTORY_ENTRIES(egammaPerformance) {
    DECLARE_TOOL     ( egammaMonTool                )
    DECLARE_TOOL     ( photonMonTool                )
    DECLARE_TOOL     ( electronMonTool              )
    DECLARE_TOOL     ( forwardEgammaMonTool         )
    DECLARE_TOOL     ( physicsMonTool               )
    DECLARE_TOOL     ( ephysicsMonTool              )
    DECLARE_TOOL     ( TopphysicsMonTool            )
    DECLARE_TOOL     ( softEMonTool                 )
}
