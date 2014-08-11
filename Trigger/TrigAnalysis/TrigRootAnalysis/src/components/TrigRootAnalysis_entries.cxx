// $Id: TrigRootAnalysis_entries.cxx 502443 2012-05-25 10:43:25Z krasznaa $

// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "../TrigConfigSvcAthD3PD.h"
#include "../TrigDecisionToolAthD3PD.h"

DECLARE_NAMESPACE_SERVICE_FACTORY( D3PD, TrigConfigSvcAthD3PD )
DECLARE_NAMESPACE_TOOL_FACTORY   ( D3PD, TrigDecisionToolAthD3PD )

DECLARE_FACTORY_ENTRIES( TrigRootAnalysis ) {

   DECLARE_NAMESPACE_SERVICE( D3PD, TrigConfigSvcAthD3PD )
   DECLARE_NAMESPACE_TOOL   ( D3PD, TrigDecisionToolAthD3PD )

}
