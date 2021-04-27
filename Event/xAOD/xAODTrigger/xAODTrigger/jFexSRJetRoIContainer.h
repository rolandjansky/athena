/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGGER_JFEXSRJETROICONTAINER_H
#define XAODTRIGGER_JFEXSRJETROICONTAINER_H

// Local include(s):
#include "xAODTrigger/jFexSRJetRoI.h"
#include "xAODTrigger/versions/jFexSRJetRoIContainer_v1.h"

namespace xAOD{
   typedef jFexSRJetRoIContainer_v1 jFexSRJetRoIContainer;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::jFexSRJetRoIContainer , 1240957639 , 1 )

#endif // XAODTRIGGER_JFEXSRJETROICONTAINER_H


