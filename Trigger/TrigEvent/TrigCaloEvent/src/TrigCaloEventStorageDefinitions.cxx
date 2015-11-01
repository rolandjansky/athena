/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCaloEvent/RingerRingsContainer.h"
#include "TrigCaloEvent/TrigT2JetContainer.h"
#include "TrigCaloEvent/TrigT2MbtsBitsContainer.h"
#include "TrigCaloEvent/TrigT2ZdcSignalsContainer.h"
#include "TrigCaloEvent/TrigTauClusterContainer.h"    
#include "TrigCaloEvent/TrigTauClusterDetailsContainer.h"
#include "TrigCaloEvent/TrigEMClusterContainer.h"
#include "TrigCaloEvent/TrigCaloClusterContainer.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloCellLinkContainer.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloShowerContainer.h"
#include "CaloEvent/CaloTowerContainer.h"
#include "TrigCaloEvent/TrigRNNOutputContainer.h"
#include "CaloEvent/CaloCell2ClusterMap.h"
#include "CaloEvent/CaloTopoTowerContainer.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "xAODTrigCalo/TrigCaloClusterContainer.h"
#include "xAODTrigRinger/TrigRingerRingsContainer.h"
#include "xAODTrigRinger/TrigRNNOutputContainer.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"
#include "xAODHIEvent/HIEventShapeContainer.h"

#include "TrigNavigation/TypeRegistration.h"
REGISTER_PACKAGE_WITH_NAVI(TrigCaloEvent)
