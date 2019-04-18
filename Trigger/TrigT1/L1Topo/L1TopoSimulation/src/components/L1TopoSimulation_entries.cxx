#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../L1TopoSimulation.h"
#include "../EMTauInputProvider.h"
#include "../EMTauInputProviderFEX.h"   
#include "../JetInputProvider.h"
#include "../JetInputProviderFEX.h"
#include "../EnergyInputProvider.h"
#include "../EnergyInputProviderFEX.h"
#include "../MuonInputProvider.h"
#include "../RoiB2TopoInputDataCnv.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY( LVL1, L1TopoSimulation )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( LVL1, RoiB2TopoInputDataCnv )

DECLARE_NAMESPACE_TOOL_FACTORY( LVL1, EMTauInputProvider )
DECLARE_NAMESPACE_TOOL_FACTORY( LVL1, EMTauInputProviderFEX )   
DECLARE_NAMESPACE_TOOL_FACTORY( LVL1, JetInputProvider )
DECLARE_NAMESPACE_TOOL_FACTORY( LVL1, JetInputProviderFEX )
DECLARE_NAMESPACE_TOOL_FACTORY( LVL1, EnergyInputProvider )
DECLARE_NAMESPACE_TOOL_FACTORY( LVL1, EnergyInputProviderFEX )
DECLARE_NAMESPACE_TOOL_FACTORY( LVL1, MuonInputProvider )

DECLARE_FACTORY_ENTRIES( L1TopoSimulation ) {

   DECLARE_NAMESPACE_ALGORITHM( LVL1, L1TopoSimulation )
   DECLARE_NAMESPACE_ALGORITHM( LVL1, RoiB2TopoInputDataCnv )

   DECLARE_NAMESPACE_TOOL( LVL1, EMTauInputProvider )
   DECLARE_NAMESPACE_TOOL( LVL1, EMTauInputProviderFEX ) 
   DECLARE_NAMESPACE_TOOL( LVL1, JetInputProvider )
   DECLARE_NAMESPACE_TOOL( LVL1, JetInputProviderFEX )
   DECLARE_NAMESPACE_TOOL( LVL1, EnergyInputProvider )
   DECLARE_NAMESPACE_TOOL( LVL1, EnergyInputProviderFEX )
   DECLARE_NAMESPACE_TOOL( LVL1, MuonInputProvider )

}
