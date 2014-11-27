// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigEMCluster.h 631374 2014-11-26 20:59:18Z gwatts $
#ifndef XAODTRIGCALO_TRIGEMCLUSTER_H
#define XAODTRIGCALO_TRIGEMCLUSTER_H

// Local include(s):
#include "xAODTrigCalo/versions/TrigEMCluster_v1.h"

/// Namespace holding all the xAOD EDM classes
namespace xAOD {
  /// Define the latest version of the trigger EM cluster class
  typedef TrigEMCluster_v1 TrigEMCluster;
}

// Set up a CLID for the object:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TrigEMCluster, 226775784, 1 )

#endif // XAODTRIGCALO_TRIGEMCLUSTER_H
