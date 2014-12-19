// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloClusterContainer.h 628097 2014-11-13 09:24:54Z krasznaa $
#ifndef XAODCALOEVENT_CALOCLUSTERCONTAINER_H
#define XAODCALOEVENT_CALOCLUSTERCONTAINER_H

// Local include(s):
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/versions/CaloClusterContainer_v1.h"

namespace xAOD {
   /// Define the latest version of the calorimeter cluster container
   typedef CaloClusterContainer_v1 CaloClusterContainer;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::CaloClusterContainer, 1219821989, 1 )

#endif // XAODCALOEVENT_CALOCLUSTERCONTAINER_H
