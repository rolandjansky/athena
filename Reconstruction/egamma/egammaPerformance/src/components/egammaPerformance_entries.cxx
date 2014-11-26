#include "egammaPerformance/photonMonTool.h"
#include "egammaPerformance/electronMonTool.h"
#include "egammaPerformance/forwardElectronMonTool.h"
#include "egammaPerformance/ZeeTaPMonTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY     ( photonMonTool            )
DECLARE_TOOL_FACTORY     ( electronMonTool          )
DECLARE_TOOL_FACTORY     ( forwardElectronMonTool   )
DECLARE_TOOL_FACTORY     ( ZeeTaPMonTool            )
DECLARE_FACTORY_ENTRIES(egammaPerformance) {
    DECLARE_TOOL     ( photonMonTool                )
    DECLARE_TOOL     ( electronMonTool              )
    DECLARE_TOOL     ( forwardElectronMonTool       )
    DECLARE_TOOL     ( ZeeTaPMonTool                )
}
