#include "../DeadMaterialShowerTool.h"
#include "../SimpleFastKillerTool.h"
#ifndef SIMULATIONBASE
#include "../FastCaloSimTool.h"
#endif

DECLARE_COMPONENT( DeadMaterialShowerTool )
DECLARE_COMPONENT( SimpleFastKillerTool )
#ifndef SIMULATIONBASE
DECLARE_COMPONENT( FastCaloSimTool )
#endif
