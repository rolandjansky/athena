// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetRoIAuxContainer.h 635056 2014-12-10 15:35:39Z watsona $
#ifndef XAODTRIGGER_JETROIAUXCONTAINER_H
#define XAODTRIGGER_JETROIAUXCONTAINER_H

// Local include(s):
#include "xAODTrigger/versions/JetRoIAuxContainer_v1.h"
#include "xAODTrigger/versions/JetRoIAuxContainer_v2.h"

namespace xAOD{
   typedef JetRoIAuxContainer_v2 JetRoIAuxContainer;
}

// Set up a CLID for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::JetRoIAuxContainer, 1287744961, 1 )

#endif // XAODTRIGGER_JETROIAUXCONTAINER_H
