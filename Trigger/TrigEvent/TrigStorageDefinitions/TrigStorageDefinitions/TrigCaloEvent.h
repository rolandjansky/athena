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

#include "xAODTrigCalo/CaloClusterTrigAuxContainer.h"

#include "xAODTrigCalo/TrigEMCluster.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "xAODTrigCalo/TrigEMClusterAuxContainer.h"

#include "xAODTrigMinBias/TrigT2MbtsBits.h"
#include "xAODTrigMinBias/TrigT2MbtsBitsContainer.h"
#include "xAODTrigMinBias/TrigT2MbtsBitsAuxContainer.h"

#include "xAODTrigMinBias/TrigT2ZdcSignals.h"
#include "xAODTrigMinBias/TrigT2ZdcSignalsContainer.h"
#include "xAODTrigMinBias/TrigT2ZdcSignalsAuxContainer.h"


#include "xAODTrigRinger/TrigRingerRings.h"
#include "xAODTrigRinger/TrigRingerRingsContainer.h"
#include "xAODTrigRinger/TrigRingerRingsAuxContainer.h"

#include "xAODTrigRinger/TrigRNNOutput.h"
#include "xAODTrigRinger/TrigRNNOutputContainer.h"
#include "xAODTrigRinger/TrigRNNOutputAuxContainer.h"


#include "xAODTrigL1Calo/TriggerTowerContainer.h"
#include "xAODTrigL1Calo/TriggerTowerAuxContainer.h"

#include "xAODHIEvent/HIEventShapeContainer.h"
#include "xAODHIEvent/HIEventShapeAuxContainer.h"



HLT_BEGIN_TYPE_REGISTRATION
  HLT_REGISTER_TYPE(class TrigEMCluster, class TrigEMCluster,class TrigEMClusterContainer)
  HLT_REGISTER_TYPE(class TrigCaloCluster, class TrigCaloCluster,class TrigCaloClusterContainer)
  HLT_REGISTER_TYPE(class TrigTauCluster, class TrigTauCluster,class TrigTauClusterContainer)
  HLT_REGISTER_TYPE(class TrigTauClusterDetails,class TrigTauClusterDetails,class TrigTauClusterDetailsContainer)
  HLT_REGISTER_TYPE(class RingerRings, class RingerRings,class RingerRingsContainer)  
  HLT_REGISTER_TYPE(class TrigT2Jet, class TrigT2Jet,class TrigT2JetContainer)
  HLT_REGISTER_TYPE(class TrigT2Jet, class TrigT2JetContainer,class TrigT2JetContainer)
  HLT_REGISTER_TYPE(class TrigT2MbtsBits,class TrigT2MbtsBits,class TrigT2MbtsBitsContainer)
  HLT_REGISTER_TYPE(class TrigT2ZdcSignals,class TrigT2ZdcSignals,class TrigT2ZdcSignalsContainer)
  //EF
  HLT_REGISTER_TYPE(class TrigRNNOutput, class TrigRNNOutput,class TrigRNNOutputContainer)
  HLT_REGISTER_TYPE(class CaloCell,class  CaloCellContainer,class CaloCellContainer )
  HLT_REGISTER_TYPE(class CaloCellLink,class  CaloCellLinkContainer,class CaloCellLinkContainer )
  HLT_REGISTER_TYPE(class CaloCluster, class CaloClusterContainer, class CaloClusterContainer )
  HLT_REGISTER_TYPE(class CaloClusterCellLink, class CaloClusterCellLinkContainer, class CaloClusterCellLinkContainer)
  HLT_REGISTER_TYPE(class CaloShower, class  CaloShowerContainer,class CaloShowerContainer )
  HLT_REGISTER_TYPE(class CaloTower, class  CaloTowerContainer, class CaloTowerContainer )

  HLT_REGISTER_TYPE(class CaloTopoTower, class CaloTopoTowerContainer, class CaloTopoTowerContainer )
  HLT_REGISTER_TYPE(class cannot_implement_this, class CaloCell2ClusterMap, class CaloCell2ClusterMap )

  // xAOD
  HLT_REGISTER_TYPE(xAOD::CaloCluster, xAOD::CaloClusterContainer, xAOD::CaloClusterContainer, xAOD::CaloClusterTrigAuxContainer)
  HLT_REGISTER_TYPE(xAOD::TrigCaloCluster, xAOD::TrigCaloCluster, xAOD::TrigCaloClusterContainer, xAOD::TrigCaloClusterAuxContainer)
  HLT_REGISTER_TYPE(xAOD::TrigEMCluster, xAOD::TrigEMCluster, xAOD::TrigEMClusterContainer, xAOD::TrigEMClusterAuxContainer)
  HLT_REGISTER_TYPE(xAOD::TrigEMCluster, xAOD::TrigEMClusterContainer, xAOD::TrigEMClusterContainer, xAOD::TrigEMClusterAuxContainer) 

  HLT_REGISTER_TYPE(xAOD::TrigT2MbtsBits, xAOD::TrigT2MbtsBits, xAOD::TrigT2MbtsBitsContainer, xAOD::TrigT2MbtsBitsAuxContainer)
  HLT_REGISTER_TYPE(xAOD::TrigT2ZdcSignals, xAOD::TrigT2ZdcSignals, xAOD::TrigT2ZdcSignalsContainer, xAOD::TrigT2ZdcSignalsAuxContainer)

  HLT_REGISTER_TYPE(xAOD::TrigRingerRings, xAOD::TrigRingerRings, xAOD::TrigRingerRingsContainer, xAOD::TrigRingerRingsAuxContainer)
  HLT_REGISTER_TYPE(xAOD::TrigRNNOutput, xAOD::TrigRNNOutput, xAOD::TrigRNNOutputContainer, xAOD::TrigRNNOutputAuxContainer)
  HLT_REGISTER_TYPE(xAOD::TriggerTower, xAOD::TriggerTowerContainer, xAOD::TriggerTowerContainer, xAOD::TriggerTowerAuxContainer)
  HLT_REGISTER_TYPE(xAOD::HIEventShape, xAOD::HIEventShapeContainer, xAOD::HIEventShapeContainer, xAOD::HIEventShapeAuxContainer)
HLT_END_TYPE_REGISTRATION(TrigCaloEvent)



#endif
