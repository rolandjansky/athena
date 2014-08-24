// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODCaloEventDict.h 591505 2014-04-05 14:59:16Z krasznaa $
#ifndef XAODCALOEVENT_XAODCALOEVENTDICT_H
#define XAODCALOEVENT_XAODCALOEVENTDICT_H

// System include(s):
#include <vector>
#include <bitset>

// EDM include(s):
#include "AthLinks/DataLink.h"
#include "AthLinks/ElementLink.h"

// Local include(s):
#include "xAODCaloEvent/versions/CaloCluster_v1.h"
#include "xAODCaloEvent/versions/CaloClusterContainer_v1.h"
#include "xAODCaloEvent/versions/CaloClusterAuxContainer_v1.h"

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
