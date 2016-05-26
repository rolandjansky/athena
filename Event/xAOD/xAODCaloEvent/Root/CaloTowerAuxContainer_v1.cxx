/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "xAODCaloEvent/versions/CaloTowerAuxContainer_v1.h"

xAOD::CaloTowerAuxContainer_v1::CaloTowerAuxContainer_v1()
  : xAOD::AuxContainerBase()
{ 
  AUX_VARIABLE( towerE );
  //  AUX_VARIABLE( towerEtaIdx );
  //  AUX_VARIABLE( towerPhiIdx );
}

xAOD::CaloTowerAuxContainer_v1::~CaloTowerAuxContainer_v1()
{ }

