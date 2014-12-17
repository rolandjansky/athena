// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EmTauRoIAuxContainer.h 635056 2014-12-10 15:35:39Z watsona $
#ifndef XAODTRIGGER_EMTAUROIAUXCONTAINER_H
#define XAODTRIGGER_EMTAUROIAUXCONTAINER_H

// Local include(s):
#include "xAODTrigger/versions/EmTauRoIAuxContainer_v1.h"
#include "xAODTrigger/versions/EmTauRoIAuxContainer_v2.h"

namespace xAOD{
   typedef EmTauRoIAuxContainer_v2 EmTauRoIAuxContainer;
}

// Set up a CLID for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::EmTauRoIAuxContainer, 1250802834, 1 )

#endif // XAODTRIGGER_EMTAUROIAUXCONTAINER_H
