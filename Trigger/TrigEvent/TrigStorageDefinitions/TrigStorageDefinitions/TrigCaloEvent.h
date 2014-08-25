/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGSTORAGEDEF_TRIGCALOEVENT
#define TRIGSTORAGEDEF_TRIGCALOEVENT
#include "TrigStorageDefinitions/EDM_TypeInformation.h"
#include "Navigation/NavigationDefaults.h"


#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"

#include "xAODTrigCalo/TrigCaloCluster.h"
#include "xAODTrigCalo/TrigCaloClusterContainer.h"
#include "xAODTrigCalo/TrigCaloClusterAuxContainer.h"

#include "xAODTrigCalo/TrigEMCluster.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "xAODTrigCalo/TrigEMClusterAuxContainer.h"

#include "xAODTrigMinBias/TrigT2MbtsBits.h"
#include "xAODTrigMinBias/TrigT2MbtsBitsContainer.h"
#include "xAODTrigMinBias/TrigT2MbtsBitsAuxContainer.h"


HLT_BEGIN_TYPE_REGISTRATION
  HLT_REGISTER_TYPE(struct TrigEMCluster, struct TrigEMCluster,struct TrigEMClusterContainer)
  HLT_REGISTER_TYPE(struct TrigCaloCluster, struct TrigCaloCluster,struct TrigCaloClusterContainer)
  HLT_REGISTER_TYPE(struct TrigTauCluster, struct TrigTauCluster,struct TrigTauClusterContainer)
  HLT_REGISTER_TYPE(struct TrigTauClusterDetails,struct TrigTauClusterDetails,struct TrigTauClusterDetailsContainer)
  HLT_REGISTER_TYPE(struct RingerRings, struct RingerRings,struct RingerRingsContainer)  
  HLT_REGISTER_TYPE(struct TrigT2Jet, struct TrigT2Jet,struct TrigT2JetContainer)
  HLT_REGISTER_TYPE(struct TrigT2Jet, struct TrigT2JetContainer,struct TrigT2JetContainer)
  HLT_REGISTER_TYPE(struct TrigT2MbtsBits,struct TrigT2MbtsBits,struct TrigT2MbtsBitsContainer)
  HLT_REGISTER_TYPE(struct TrigT2ZdcSignals,struct TrigT2ZdcSignals,struct TrigT2ZdcSignalsContainer)
  //EF
  HLT_REGISTER_TYPE(struct TrigRNNOutput, struct TrigRNNOutput,struct TrigRNNOutputContainer)
  HLT_REGISTER_TYPE(struct CaloCell,struct  CaloCellContainer,struct CaloCellContainer )
  HLT_REGISTER_TYPE(struct CaloCellLink,struct  CaloCellLinkContainer,struct CaloCellLinkContainer )
  HLT_REGISTER_TYPE(struct CaloCluster, struct CaloClusterContainer, struct CaloClusterContainer )
  HLT_REGISTER_TYPE(struct CaloClusterCellLink, struct CaloClusterCellLinkContainer, struct CaloClusterCellLinkContainer)
  HLT_REGISTER_TYPE(struct CaloShower, struct  CaloShowerContainer,struct CaloShowerContainer )
  HLT_REGISTER_TYPE(struct CaloTower, struct  CaloTowerContainer, struct CaloTowerContainer )

  HLT_REGISTER_TYPE(struct CaloTopoTower, struct CaloTopoTowerContainer, struct CaloTopoTowerContainer )
  HLT_REGISTER_TYPE(struct cannot_implement_this, struct CaloCell2ClusterMap, struct CaloCell2ClusterMap )

  // xAOD
  HLT_REGISTER_TYPE(xAOD::CaloCluster, xAOD::CaloClusterContainer, xAOD::CaloClusterContainer, xAOD::CaloClusterAuxContainer)
  HLT_REGISTER_TYPE(xAOD::TrigCaloCluster, xAOD::TrigCaloCluster, xAOD::TrigCaloClusterContainer, xAOD::TrigCaloClusterAuxContainer)
  HLT_REGISTER_TYPE(xAOD::TrigEMCluster, xAOD::TrigEMCluster, xAOD::TrigEMClusterContainer, xAOD::TrigEMClusterAuxContainer)
  HLT_REGISTER_TYPE(xAOD::TrigEMCluster, xAOD::TrigEMClusterContainer, xAOD::TrigEMClusterContainer, xAOD::TrigEMClusterAuxContainer) 
  HLT_REGISTER_TYPE(xAOD::TrigT2MbtsBits, xAOD::TrigT2MbtsBits, xAOD::TrigT2MbtsBitsContainer, xAOD::TrigT2MbtsBitsAuxContainer)

HLT_END_TYPE_REGISTRATION(TrigCaloEvent)


#endif
