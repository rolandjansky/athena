/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGGER_JFEXLRJETROICONTAINER_H
#define XAODTRIGGER_JFEXLRJETROICONTAINER_H

// Local include(s):
#include "xAODTrigger/jFexLRJetRoI.h"
#include "xAODTrigger/versions/jFexLRJetRoIContainer_v1.h"

namespace xAOD{
   typedef jFexLRJetRoIContainer_v1 jFexLRJetRoIContainer;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::jFexLRJetRoIContainer , 1251138464 , 1 )
#endif // XAODTRIGGER_JFEXLRJETROICONTAINER_H



