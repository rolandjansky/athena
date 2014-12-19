// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloClusterAuxContainer.h 628097 2014-11-13 09:24:54Z krasznaa $
#ifndef XAODCALOEVENT_CALOCLUSTERAUXCONTAINER_H
#define XAODCALOEVENT_CALOCLUSTERAUXCONTAINER_H

// Local include(s):
#include "xAODCaloEvent/versions/CaloClusterAuxContainer_v2.h"

namespace xAOD {
   /// Define the latest version of the calorimeter cluster auxiliary container
   typedef CaloClusterAuxContainer_v2 CaloClusterAuxContainer;

}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::CaloClusterAuxContainer, 1302486236, 1 )

#endif // XAODCALOEVENT_CALOCLUSTERAUXCONTAINER_H
