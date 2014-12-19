// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODCaloEventDict.h 636408 2014-12-17 05:43:51Z ssnyder $
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

   };
}

#endif // XAODCALOEVENT_XAODCALOEVENTDICT_H
