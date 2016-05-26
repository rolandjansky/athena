/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ContainerProxies.cxx 700669 2015-10-15 08:31:13Z wlampl $

// EDM include(s):
#include "xAODCore/AddDVProxy.h"

// Local include(s):
#include "xAODCaloEvent/versions/CaloClusterContainer_v1.h"
#include "xAODCaloEvent/versions/CaloTowerContainer_v1.h"

// Set up the collection proxies:
ADD_NS_DV_PROXY( xAOD, CaloClusterContainer_v1 );
//ADD_NS_DV_PROXY( xAOD, DataVector<xAOD::CaloTower_v1> );
ADD_NS_DV_PROXY( xAOD, CaloTowerContainerBase_v1 );
