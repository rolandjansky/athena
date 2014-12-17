// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetRoIContainer.h 635056 2014-12-10 15:35:39Z watsona $
#ifndef XAODTRIGGER_JETROICONTAINER_H
#define XAODTRIGGER_JETROICONTAINER_H

// Local include(s):
#include "xAODTrigger/JetRoI.h"
#include "xAODTrigger/versions/JetRoIContainer_v1.h"
#include "xAODTrigger/versions/JetRoIContainer_v2.h"

namespace xAOD{
   typedef JetRoIContainer_v2 JetRoIContainer;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::JetRoIContainer, 1132837218, 1 )

#endif // XAODTRIGGER_JETROICONTAINER_H
