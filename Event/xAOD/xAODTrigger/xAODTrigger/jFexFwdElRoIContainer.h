/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGGER_JFEXFWDELROICONTAINER_H
#define XAODTRIGGER_JFEXFWDELROICONTAINER_H

// Local include(s):
#include "xAODTrigger/jFexFwdElRoI.h"
#include "xAODTrigger/versions/jFexFwdElRoIContainer_v1.h"

namespace xAOD{
   typedef jFexFwdElRoIContainer_v1 jFexFwdElRoIContainer;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::jFexFwdElRoIContainer , 1130400193 , 1 )

#endif // XAODTRIGGER_JFEXFWDROICONTAINER_H


