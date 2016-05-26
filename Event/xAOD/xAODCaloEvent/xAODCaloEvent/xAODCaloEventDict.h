// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODCaloEventDict.h 700669 2015-10-15 08:31:13Z wlampl $
#ifndef XAODCALOEVENT_XAODCALOEVENTDICT_H
#define XAODCALOEVENT_XAODCALOEVENTDICT_H

// Needed to successfully generate the dictionary in standalone mode:
#if defined(__GCCXML__) and not defined(EIGEN_DONT_VECTORIZE)
#   define EIGEN_DONT_VECTORIZE
#endif // __GCCXML__


// System include(s):
#include <vector>
#include <bitset>

// EDM include(s):
#include "AthLinks/DataLink.h"
#include "AthLinks/ElementLink.h"

// Local include(s):
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/versions/CaloCluster_v1.h"
#include "xAODCaloEvent/versions/CaloClusterContainer_v1.h"
#include "xAODCaloEvent/versions/CaloClusterAuxContainer_v1.h"
#include "xAODCaloEvent/versions/CaloClusterAuxContainer_v2.h"
#include "xAODCaloEvent/CaloVertexedClusterBase.h"
#include "xAODCaloEvent/CaloVertexedTopoCluster.h"

//#include "xAODCaloEvent/CaloTowerDescriptor.h"
//#include "xAODCaloEvent/CaloTowerGrid.h"
#include "xAODCaloEvent/CaloTower.h"
#include "xAODCaloEvent/CaloTowerContainer.h"
#include "xAODCaloEvent/CaloTowerAuxContainer.h"
#include "xAODCaloEvent/versions/CaloTower_v1.h"
#include "xAODCaloEvent/versions/CaloTowerContainer_v1.h"
#include "xAODCaloEvent/versions/CaloTowerAuxContainer_v1.h"


namespace {
   struct GCCXML_DUMMY_INSTANTIATION_XAODCALOEVENT {
      // Container(s):
      xAOD::CaloClusterContainer_v1 c1;
      // Data link(s):
      DataLink< xAOD::CaloClusterContainer_v1 > dl1;
     std::vector< DataLink< xAOD::CaloClusterContainer_v1 > > dl2;
      // Element link(s):
      ElementLink< xAOD::CaloClusterContainer_v1 > el1;
      std::vector< ElementLink< xAOD::CaloClusterContainer_v1 > > el2;
      std::vector< std::vector< ElementLink< xAOD::CaloClusterContainer_v1 > > > el3;
      // Additional type(s) needed:
      std::bitset< 3 > type1; // Using 3 instead of CaloCluster::NSTATES...

     //BadChannel lists 
     xAOD::CaloClusterBadChannelData_v1 bcd;
     std::vector<xAOD::CaloClusterBadChannelData_v1> vbcd;
     std::vector<std::vector<xAOD::CaloClusterBadChannelData_v1> > vvbcd;

     // CaloTower containers
     xAOD::CaloTowerContainer_v1 t1;
     xAOD::CaloTowerContainerBase_v1 dvt;
     // Data link(s)
     DataLink< xAOD::CaloTowerContainer_v1 > tdl1;
     std::vector< DataLink< xAOD::CaloTowerContainer_v1 > > tdl2;
     // Element link(s)
     ElementLink< xAOD::CaloTowerContainer_v1 > tel1;
     std::vector< ElementLink< xAOD::CaloTowerContainer_v1 > > tel2;
     std::vector< std::vector< ElementLink< xAOD::CaloTowerContainer_v1 > > > tel3;

   };
}

#endif // XAODCALOEVENT_XAODCALOEVENTDICT_H
