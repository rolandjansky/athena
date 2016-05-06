// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PFOContainer.h 744541 2016-05-03 15:55:30Z krasznaa $
#ifndef XAODPFLOW_PFOCONTAINER_H
#define XAODPFLOW_PFOCONTAINER_H

// Local include(s):
#include "xAODPFlow/PFO.h"
#include "xAODPFlow/versions/PFOContainer_v1.h"

namespace xAOD {
   /// Definition of the current "pfo container version"
   typedef PFOContainer_v1 PFOContainer;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::PFOContainer, 1177172577, 1 )

#endif // XAODPFLOW_PFOCONTAINER_H
