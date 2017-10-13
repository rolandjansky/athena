#include "TrigT1CaloTools/L1DatabaseOverrideForDataTool.h"
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
// #include "TrigT1CaloTools/L1RoITools.h"
#include "TrigT1CaloTools/L1CPMTowerTools.h"
#include "TrigT1CaloTools/L1CPMTools.h"
#include "TrigT1CaloTools/L1JEMJetTools.h"
#include "TrigT1CaloTools/L1CPCMXTools.h"
#include "TrigT1CaloTools/L1EnergyCMXTools.h"
#include "TrigT1CaloTools/L1JetCMXTools.h"
#include "TrigT1CaloTools/L1NonlinearLutDatabaseOverrideTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY( LVL1, L1DatabaseOverrideForDataTool )
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
// DECLARE_NAMESPACE_TOOL_FACTORY( LVL1, L1RoITools )
DECLARE_NAMESPACE_TOOL_FACTORY( LVL1, L1CPMTowerTools )
DECLARE_NAMESPACE_TOOL_FACTORY( LVL1, L1CPMTools )
DECLARE_NAMESPACE_TOOL_FACTORY( LVL1, L1JEMJetTools )
DECLARE_NAMESPACE_TOOL_FACTORY( LVL1, L1CPCMXTools )
DECLARE_NAMESPACE_TOOL_FACTORY( LVL1, L1EnergyCMXTools )
DECLARE_NAMESPACE_TOOL_FACTORY( LVL1, L1JetCMXTools )
DECLARE_NAMESPACE_TOOL_FACTORY( LVL1, L1NonlinearLutDatabaseOverrideTool )

