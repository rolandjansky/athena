/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SGTools/BaseInfo.h"
#include "TrigCaloEvent/TrigCaloCluster.h"
#include "TrigCaloEvent/TrigEMCluster.h"
#include "TrigT1CaloEvent/TriggerTowerCollection.h"

SG_ADD_BASE(TrigEMCluster, TrigCaloCluster);
SG_ADD_BASE (TriggerTowerCollection, DataVector<TriggerTower> );

