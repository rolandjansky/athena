#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrigT1CaloTools/L1DynamicPedestalProviderTxt.h"
#include "TrigT1CaloTools/L1DynamicPedestalProviderRoot.h"
#include "TrigT1CaloTools/L1EmTauTools.h"
#include "TrigT1CaloTools/L1JetTools.h"
#include "TrigT1CaloTools/L1EtTools.h"
#include "TrigT1CaloTools/L1JetElementTools.h"
#include "TrigT1CaloTools/L1CPHitsTools.h"
#include "TrigT1CaloTools/L1JEPHitsTools.h"
#include "TrigT1CaloTools/L1JEPEtSumsTools.h"
#include "TrigT1CaloTools/L1JetEtTools.h"
#include "TrigT1CaloTools/L1TriggerTowerTool.h"
#include "TrigT1CaloTools/L1RoITools.h"

DECLARE_NAMESPACE_TOOL_FACTORY( LVL1, L1DynamicPedestalProviderTxt )
DECLARE_NAMESPACE_TOOL_FACTORY( LVL1, L1DynamicPedestalProviderRoot )
DECLARE_NAMESPACE_TOOL_FACTORY( LVL1, L1EmTauTools )
DECLARE_NAMESPACE_TOOL_FACTORY( LVL1, L1JetTools )
DECLARE_NAMESPACE_TOOL_FACTORY( LVL1, L1EtTools )
DECLARE_NAMESPACE_TOOL_FACTORY( LVL1, L1JetElementTools )
DECLARE_NAMESPACE_TOOL_FACTORY( LVL1, L1CPHitsTools )
DECLARE_NAMESPACE_TOOL_FACTORY( LVL1, L1JEPHitsTools )
DECLARE_NAMESPACE_TOOL_FACTORY( LVL1, L1JEPEtSumsTools )
DECLARE_NAMESPACE_TOOL_FACTORY( LVL1, L1JetEtTools )
DECLARE_NAMESPACE_TOOL_FACTORY( LVL1, L1TriggerTowerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( LVL1, L1RoITools )

DECLARE_FACTORY_ENTRIES( TrigT1CaloTools )
{
	DECLARE_NAMESPACE_TOOL( LVL1, L1DynamicPedestalProviderTxt )
	DECLARE_NAMESPACE_TOOL( LVL1, L1DynamicPedestalProviderRoot )
	DECLARE_NAMESPACE_TOOL( LVL1, L1EmTauTools )
        DECLARE_NAMESPACE_TOOL( LVL1, L1JetTools )
        DECLARE_NAMESPACE_TOOL( LVL1, L1EtTools )
        DECLARE_NAMESPACE_TOOL( LVL1, L1JetElementTools )
	DECLARE_NAMESPACE_TOOL( LVL1, L1CPHitsTools )
	DECLARE_NAMESPACE_TOOL( LVL1, L1JEPHitsTools )
	DECLARE_NAMESPACE_TOOL( LVL1, L1JEPEtSumsTools )
        DECLARE_NAMESPACE_TOOL( LVL1, L1JetEtTools )
        DECLARE_NAMESPACE_TOOL( LVL1, L1TriggerTowerTool )
	DECLARE_NAMESPACE_TOOL( LVL1, L1RoITools )
}

