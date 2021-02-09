// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGGER_GFEXGLOBALROICONTAINER_H
#define XAODTRIGGER_GFEXGLOBALROICONTAINER_H

// Local include(s):
#include "xAODTrigger/gFexGlobalRoI.h"
#include "xAODTrigger/versions/gFexGlobalRoIContainer_v1.h"

namespace xAOD{
   typedef gFexGlobalRoIContainer_v1 gFexGlobalRoIContainer;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::gFexGlobalRoIContainer , 1186121853 , 1 )

#endif // XAODTRIGGER_GFEXGLOBALROICONTAINER_H
