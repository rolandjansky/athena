
#include "../L1TopoSimulation.h"
#include "../EMTauInputProvider.h"
#include "../JetInputProvider.h"
#include "../EnergyInputProvider.h"
#include "../MuonInputProvider.h"
#include "../RoiB2TopoInputDataCnv.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY( LVL1, L1TopoSimulation )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( LVL1, RoiB2TopoInputDataCnv )

DECLARE_NAMESPACE_TOOL_FACTORY( LVL1, EMTauInputProvider )
DECLARE_NAMESPACE_TOOL_FACTORY( LVL1, JetInputProvider )
DECLARE_NAMESPACE_TOOL_FACTORY( LVL1, EnergyInputProvider )
DECLARE_NAMESPACE_TOOL_FACTORY( LVL1, MuonInputProvider )

