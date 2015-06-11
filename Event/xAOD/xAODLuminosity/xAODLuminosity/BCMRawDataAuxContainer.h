// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BCMRawDataAuxContainer.h 652111 2015-03-06 10:13:22Z krasznaa $
#ifndef XAODLUMINOSITY_BCMRAWDATAAUXCONTAINER_H
#define XAODLUMINOSITY_BCMRAWDATAAUXCONTAINER_H

// Local include(s):
#include "xAODLuminosity/versions/BCMRawDataAuxContainer_v1.h"

namespace xAOD {
   /// Declare the latest version of the auxiliary container
   typedef BCMRawDataAuxContainer_v1 BCMRawDataAuxContainer;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::BCMRawDataAuxContainer, 1252363455, 1 )

#endif // XAODLUMINOSITY_BCMRAWDATAAUXCONTAINER_H
