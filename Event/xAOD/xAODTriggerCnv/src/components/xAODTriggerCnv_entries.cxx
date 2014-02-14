// $Id: xAODTriggerCnv_entries.cxx 583381 2014-02-14 15:58:20Z krasznaa $

// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "../MuonRoICnvTool.h"
#include "../MuonRoICnvAlg.h"
#include "../JetRoICnvTool.h"
#include "../JetRoICnvAlg.h"
#include "../EmTauRoICnvTool.h"
#include "../EmTauRoICnvAlg.h"
#include "../EnergySumRoICnvTool.h"
#include "../EnergySumRoICnvAlg.h"
#include "../JetEtRoICnvTool.h"
#include "../JetEtRoICnvAlg.h"

#include "../TrigDecisionCnvTool.h"
#include "../TrigDecisionCnvAlg.h"

#include "../BunchConfCnvAlg.h"

DECLARE_NAMESPACE_TOOL_FACTORY( xAODMaker, MuonRoICnvTool )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODMaker, MuonRoICnvAlg )
DECLARE_NAMESPACE_TOOL_FACTORY( xAODMaker, JetRoICnvTool )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODMaker, JetRoICnvAlg )
DECLARE_NAMESPACE_TOOL_FACTORY( xAODMaker, EmTauRoICnvTool )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODMaker, EmTauRoICnvAlg )
DECLARE_NAMESPACE_TOOL_FACTORY( xAODMaker, EnergySumRoICnvTool )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODMaker, EnergySumRoICnvAlg )
DECLARE_NAMESPACE_TOOL_FACTORY( xAODMaker, JetEtRoICnvTool )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODMaker, JetEtRoICnvAlg )

DECLARE_NAMESPACE_TOOL_FACTORY( xAODMaker, TrigDecisionCnvTool )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODMaker, TrigDecisionCnvAlg )

DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODMaker, BunchConfCnvAlg )

DECLARE_FACTORY_ENTRIES( xAODAnalysisTriggerCnv ) {

   DECLARE_NAMESPACE_TOOL( xAODMaker, MuonRoICnvTool )
   DECLARE_NAMESPACE_ALGORITHM( xAODMaker, MuonRoICnvAlg )
   DECLARE_NAMESPACE_TOOL( xAODMaker, JetRoICnvTool )
   DECLARE_NAMESPACE_ALGORITHM( xAODMaker, JetRoICnvAlg )
   DECLARE_NAMESPACE_TOOL( xAODMaker, EmTauRoICnvTool )
   DECLARE_NAMESPACE_ALGORITHM( xAODMaker, EmTauRoICnvAlg )
   DECLARE_NAMESPACE_TOOL( xAODMaker, EnergySumRoICnvTool )
   DECLARE_NAMESPACE_ALGORITHM( xAODMaker, EnergySumRoICnvAlg )
   DECLARE_NAMESPACE_TOOL( xAODMaker, JetEtRoICnvTool )
   DECLARE_NAMESPACE_ALGORITHM( xAODMaker, JetEtRoICnvAlg )

   DECLARE_NAMESPACE_TOOL( xAODMaker, TrigDecisionCnvTool )
   DECLARE_NAMESPACE_ALGORITHM( xAODMaker, TrigDecisionCnvAlg )

   DECLARE_NAMESPACE_ALGORITHM( xAODMaker, BunchConfCnvAlg )

}
