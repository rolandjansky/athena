// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloClusterAuxContainer.h 628097 2014-11-13 09:24:54Z krasznaa $
#ifndef XAODTRIGCALO_CALOCLUSTERTRIGAUXCONTAINER_H
#define XAODTRIGCALO_CALOCLUSTERTRIGAUXCONTAINER_H

// Local include(s):
#include "xAODTrigCalo/versions/CaloClusterTrigAuxContainer_v1.h"

namespace xAOD {
   /// Define the latest version of the calorimeter cluster auxiliary container
   typedef CaloClusterTrigAuxContainer_v1 CaloClusterTrigAuxContainer;

}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::CaloClusterTrigAuxContainer, 1160663939, 1 )

#endif // XAODTRIGCALO_CALOCLUSTERTRIGAUXCONTAINER_H
