/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "../HLTCalo_L2CaloEMClustersMonitor.h"
#include "../HLTCalo_TopoCaloClustersMonitor.h"
#include "TrigCaloMonitoring/HLTCaloToolL2.h"
#include "TrigCaloMonitoring/HLTCaloClusterTool.h"
#include "TrigCaloMonitoring/HLTCaloESD_xAODTrigEMClusters.h"
#include "TrigCaloMonitoring/HLTCaloESD_xAODCaloClusters.h"
#include "TrigCaloMonitoring/HLTCaloESD_CaloCells.h"


DECLARE_COMPONENT( HLTCalo_L2CaloEMClustersMonitor )
DECLARE_COMPONENT( HLTCalo_TopoCaloClustersMonitor )
DECLARE_COMPONENT( HLTCaloToolL2 )
DECLARE_COMPONENT( HLTCaloClusterTool )
DECLARE_COMPONENT( HLTCaloESD_xAODTrigEMClusters )
DECLARE_COMPONENT( HLTCaloESD_xAODCaloClusters )
DECLARE_COMPONENT( HLTCaloESD_CaloCells )

