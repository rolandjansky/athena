// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BCMRawDataContainer.h 652111 2015-03-06 10:13:22Z krasznaa $
#ifndef XAODLUMINOSITY_BCMRAWDATACONTAINER_H
#define XAODLUMINOSITY_BCMRAWDATACONTAINER_H

// Local include(s):
#include "xAODLuminosity/versions/BCMRawDataContainer_v1.h"
#include "xAODLuminosity/BCMRawData.h"

namespace xAOD {
   /// Declare the latest version of the container
   typedef BCMRawDataContainer_v1 BCMRawDataContainer;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::BCMRawDataContainer, 1213590216, 1 )

#endif // XAODLUMINOSITY_BCMRAWDATACONTAINER_H
