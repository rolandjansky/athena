// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EmTauRoIContainer.h 635056 2014-12-10 15:35:39Z watsona $
#ifndef XAODTRIGGER_EMTAUROICONTAINER_H
#define XAODTRIGGER_EMTAUROICONTAINER_H

// Local include(s):
#include "xAODTrigger/EmTauRoI.h"
#include "xAODTrigger/versions/EmTauRoIContainer_v1.h"
#include "xAODTrigger/versions/EmTauRoIContainer_v2.h"

namespace xAOD{
   typedef EmTauRoIContainer_v2 EmTauRoIContainer;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::EmTauRoIContainer, 1078901071, 1 )

#endif // XAODTRIGGER_EMTAUROICONTAINER_H
