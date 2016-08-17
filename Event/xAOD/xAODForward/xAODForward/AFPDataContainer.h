// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: AFPDataContainer.h 693858 2015-09-09 10:30:15Z krasznaa $
#ifndef XAODFORWARD_AFPDATACONTAINER_H
#define XAODFORWARD_AFPDATACONTAINER_H

// EDM include(s):
#include "xAODCore/CLASS_DEF.h"

// Local include(s):
#include "xAODForward/AFPData.h"
#include "xAODForward/versions/AFPDataContainer_v1.h"

namespace xAOD
{
   typedef AFPDataContainer_v1 AFPDataContainer;
}

// Set up a CLID for the container:
CLASS_DEF( xAOD::AFPDataContainer, 1167803130, 1 )

#endif // XAODFORWARD_AFPDATACONTAINER_H
