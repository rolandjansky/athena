// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloClusterContainer.h 563268 2013-09-27 07:40:29Z krasznaa $
#ifndef XAODCALOEVENT_CALOCLUSTERCONTAINER_H
#define XAODCALOEVENT_CALOCLUSTERCONTAINER_H

// Local include(s):
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/versions/CaloClusterContainer_v1.h"

namespace xAOD {
   /// Define the latest version of the calorimeter cluster container
   typedef CaloClusterContainer_v1 CaloClusterContainer;
}

#endif // XAODCALOEVENT_CALOCLUSTERCONTAINER_H
