/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGGER_JFEXSUMETROICONTAINER_H
#define XAODTRIGGER_JFEXSUMETROICONTAINER_H

// Local include(s):
#include "xAODTrigger/jFexSumETRoI.h"
#include "xAODTrigger/versions/jFexSumETRoIContainer_v1.h"

namespace xAOD{
   typedef jFexSumETRoIContainer_v1 jFexSumETRoIContainer;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::jFexSumETRoIContainer , 1334096337 , 1 )
#endif // XAODTRIGGER_JFEXSUMETROICONTAINER_H


