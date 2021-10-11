/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGGER_JFEXTAUROICONTAINER_H
#define XAODTRIGGER_JFEXTAUROICONTAINER_H

// Local include(s):
#include "xAODTrigger/jFexTauRoI.h"
#include "xAODTrigger/versions/jFexTauRoIContainer_v1.h"

namespace xAOD{
   typedef jFexTauRoIContainer_v1 jFexTauRoIContainer;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::jFexTauRoIContainer , 1339336323 , 1 )

#endif // XAODTRIGGER_JFEXTAUROICONTAINER_H


