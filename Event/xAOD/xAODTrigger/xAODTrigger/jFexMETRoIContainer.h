/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGGER_JFEXMETROICONTAINER_H
#define XAODTRIGGER_JFEXMETROICONTAINER_H

// Local include(s):
#include "xAODTrigger/jFexMETRoI.h"
#include "xAODTrigger/versions/jFexMETRoIContainer_v1.h"

namespace xAOD{
   typedef jFexMETRoIContainer_v1 jFexMETRoIContainer;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::jFexMETRoIContainer , 1122466163 , 1 )
#endif // XAODTRIGGER_JFEXMETROICONTAINER_H


