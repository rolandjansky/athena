// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODFORWARD_MBTSMODULESCONTAINER_H
#define XAODFORWARD_MBTSMODULESCONTAINER_H

// EDM include(s):
#include "xAODCore/CLASS_DEF.h"

// Local include(s):
#include "xAODForward/MBTSModule.h"
#include "xAODForward/versions/MBTSModuleContainer_v1.h"

namespace xAOD {
   typedef MBTSModuleContainer_v1 MBTSModuleContainer;
}

// Set up a CLID for the container:
CLASS_DEF( xAOD::MBTSModuleContainer, 1338382772, 1 )

#endif // XAODFORWARD_MBTSMODULESCONTAINER_H
