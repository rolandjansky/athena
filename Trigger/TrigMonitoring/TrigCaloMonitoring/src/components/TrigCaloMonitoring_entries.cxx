
#include "TrigCaloMonitoring/HLTCaloTool.h"
#include "TrigCaloMonitoring/HLTCaloFEBTool.h"
#include "TrigCaloMonitoring/HLTCaloToolL2.h"
#include "TrigCaloMonitoring/HLTCaloClusterTool.h"
#include "TrigCaloMonitoring/HLTCaloESD_xAODTrigEMClusters.h"
#include "TrigCaloMonitoring/HLTCaloESD_xAODCaloClusters.h"
#include "TrigCaloMonitoring/HLTCaloESD_CaloCells.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY(HLTCaloTool )
DECLARE_TOOL_FACTORY(HLTCaloFEBTool )
DECLARE_TOOL_FACTORY(HLTCaloToolL2 )
DECLARE_TOOL_FACTORY( HLTCaloClusterTool )
DECLARE_TOOL_FACTORY( HLTCaloESD_xAODTrigEMClusters)
DECLARE_TOOL_FACTORY( HLTCaloESD_xAODCaloClusters)
DECLARE_TOOL_FACTORY( HLTCaloESD_CaloCells)

DECLARE_FACTORY_ENTRIES(TrigCaloMonitoring){
  DECLARE_ALGTOOL( HLTCaloTool )
  DECLARE_ALGTOOL( HLTCaloFEBTool )
  DECLARE_ALGTOOL( HLTCaloToolL2 )
  DECLARE_ALGTOOL( HLTCaloClusterTool )
  DECLARE_ALGTOOL( HLTCaloESD_xAODTrigEMClusters )
  DECLARE_ALGTOOL( HLTCaloESD_xAODCaloClusters )
  DECLARE_ALGTOOL( HLTCaloESD_CaloCells )
}
