// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloCluster.h 628097 2014-11-13 09:24:54Z krasznaa $
#ifndef XAODCALOEVENT_CALOCLUSTER_H
#define XAODCALOEVENT_CALOCLUSTER_H

// Local include(s):
#include "xAODCaloEvent/versions/CaloCluster_v1.h"

/// Namespace holding all the xAOD EDM classes
namespace xAOD {
   /// Define the latest version of the calorimeter cluster class
   typedef CaloCluster_v1 CaloCluster;
}

// Set up a CLID for the object:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::CaloCluster, 176433021, 1 )

#endif // XAODCALOEVENT_CALOCLUSTER_H
