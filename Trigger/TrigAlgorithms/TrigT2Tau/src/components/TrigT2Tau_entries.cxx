#include "TrigT2Tau/T2TauFinal.h"
#include "TrigT2Tau/T2TauEnergyTool.h"
#include "TrigT2Tau/T2TauEtFlowTool.h"
#include "TrigT2Tau/T2TauEtaPhiPtTool.h"
#include "TrigT2Tau/T2TauMatchingTool.h"
#include "TrigT2Tau/T2TauTrkRadiusTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( T2TauFinal )
DECLARE_TOOL_FACTORY( T2TauEnergyTool )
DECLARE_TOOL_FACTORY( T2TauEtFlowTool )
DECLARE_TOOL_FACTORY( T2TauEtaPhiPtTool )
DECLARE_TOOL_FACTORY( T2TauMatchingTool )
DECLARE_TOOL_FACTORY( T2TauTrkRadiusTool )

DECLARE_FACTORY_ENTRIES( TrigT2Tau ) {
    DECLARE_ALGORITHM( T2TauFinal );
    DECLARE_TOOL( T2TauEnergyTool );
    DECLARE_TOOL( T2TauEtFlowTool );
    DECLARE_TOOL( T2TauEtaPhiPtTool );
    DECLARE_TOOL( T2TauMatchingTool );
    DECLARE_TOOL( T2TauTrkRadiusTool );
}
