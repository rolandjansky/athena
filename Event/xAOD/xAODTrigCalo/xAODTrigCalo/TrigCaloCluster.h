// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigCaloCluster.h 631374 2014-11-26 20:59:18Z gwatts $
#ifndef XAODTRIGCALO_TRIGCALOCLUSTER_H
#define XAODTRIGCALO_TRIGCALOCLUSTER_H

// Local include(s):
#include "xAODTrigCalo/versions/TrigCaloCluster_v1.h"

/// Namespace holding all the xAOD EDM classes
namespace xAOD {
  /// Define the latest version of the trigger calorimeter cluster class
  typedef TrigCaloCluster_v1 TrigCaloCluster;
}

// Set up a CLID for the object:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TrigCaloCluster, 114069001, 1 )

#endif // XAODTRIGCALO_TRIGCALOCLUSTER_H
