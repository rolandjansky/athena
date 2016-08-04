// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloCluster.h 766396 2016-08-04 11:31:11Z wlampl $
#ifndef XAODCALOEVENT_CALOCLUSTER_H
#define XAODCALOEVENT_CALOCLUSTER_H

#include <memory>

// Local include(s):
#include "xAODCaloEvent/versions/CaloCluster_v1.h"

/// Namespace holding all the xAOD EDM classes
namespace xAOD {
   /// Define the latest version of the calorimeter cluster class
   typedef CaloCluster_v1 CaloCluster;

  //Overloaded function to allow shallow-copying CaloClusters, perserving the sampling pattern
  //See also xAODCore/ShallowCopy.h
  std::unique_ptr<CaloCluster> prepareElementForShallowCopy(const CaloCluster* orgCluster);


}

// Set up a CLID for the object:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::CaloCluster, 176433021, 1 )

#endif // XAODCALOEVENT_CALOCLUSTER_H
