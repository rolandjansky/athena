// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGGER_GFEXJETROICONTAINER_H
#define XAODTRIGGER_GFEXJETROICONTAINER_H

// Local include(s):
#include "xAODTrigger/gFexJetRoI.h"
#include "xAODTrigger/versions/gFexJetRoIContainer_v1.h"

namespace xAOD{
   typedef gFexJetRoIContainer_v1 gFexJetRoIContainer;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::gFexJetRoIContainer , 1176191653 , 1 )

#endif // XAODTRIGGER_GFEXJETRoICONTAINER_H
