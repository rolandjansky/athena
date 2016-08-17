// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: AFPDataAuxContainer.h 693858 2015-09-09 10:30:15Z krasznaa $
#ifndef XAODFORWARD_AFPDATAAUXCONTAINER_H
#define XAODFORWARD_AFPDATAAUXCONTAINER_H

// EDM include(s):
#include "xAODCore/CLASS_DEF.h"

// Local include(s):
#include "xAODForward/versions/AFPDataAuxContainer_v1.h"

namespace xAOD
{
   typedef AFPDataAuxContainer_v1 AFPDataAuxContainer;
}

// Set up a CLID for the class:
CLASS_DEF( xAOD::AFPDataAuxContainer, 1096875233, 1 )

#endif // XAODFORWARD_AFPDATAAUXCONTAINER_H
