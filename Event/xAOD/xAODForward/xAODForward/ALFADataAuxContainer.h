// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ALFADataAuxContainer.h 693858 2015-09-09 10:30:15Z krasznaa $
#ifndef XAODFORWARD_ALFADATAAUXCONTAINER_H
#define XAODFORWARD_ALFADATAAUXCONTAINER_H

// EDM include(s):
#include "xAODCore/CLASS_DEF.h"

// Local include(s):
#include "xAODForward/versions/ALFADataAuxContainer_v1.h"

namespace xAOD {
   typedef ALFADataAuxContainer_v1 ALFADataAuxContainer;
}

// Set up a CLID for the class:
CLASS_DEF( xAOD::ALFADataAuxContainer, 1197261491, 1 )

#endif // XAODFORWARD_ALFADATAAUXCONTAINER_H
