// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TriggerMenuContainer_v1.h 572671 2013-11-27 14:22:47Z krasznaa $
#ifndef XAODTRIGGER_VERSIONS_TRIGGERMENUCONTAINER_V1_H
#define XAODTRIGGER_VERSIONS_TRIGGERMENUCONTAINER_V1_H

// EDM include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODTrigger/versions/TriggerMenu_v1.h"

namespace xAOD {
   /// Define the menu container as a simple DataVector
   typedef DataVector< TriggerMenu_v1 > TriggerMenuContainer_v1;
}

// Set up a CLID for the container:
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::TriggerMenuContainer_v1, 1107011239, 1 )
#endif // not XAOD_STANDALONE

#endif // XAODTRIGGER_VERSIONS_TRIGGERMENUCONTAINER_V1_H
