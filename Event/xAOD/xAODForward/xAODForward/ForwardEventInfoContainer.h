// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODFORWARD_FORWARDEVENTINFOSCONTAINER_H
#define XAODFORWARD_FORWARDEVENTINFOSCONTAINER_H

// EDM include(s):
#include "xAODCore/CLASS_DEF.h"

// Local include(s):
#include "xAODForward/ForwardEventInfo.h"
#include "xAODForward/versions/ForwardEventInfoContainer_v1.h"

namespace xAOD {
   typedef ForwardEventInfoContainer_v1 ForwardEventInfoContainer;
}

// Set up a CLID for the container:
CLASS_DEF( xAOD::ForwardEventInfoContainer, 1121437929, 1 )

#endif // XAODFORWARD_FORWARDEVENTINFOSCONTAINER_H
