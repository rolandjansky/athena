// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: eFexEMRoIContainer.h 2019-07-01 13:04:25Z watsona $
#ifndef XAODTRIGGER_EFEXEMROICONTAINER_H
#define XAODTRIGGER_EFEXEMROICONTAINER_H

// Local include(s):
#include "xAODTrigger/eFexEMRoI.h"
#include "xAODTrigger/versions/eFexEMRoIContainer_v1.h"

namespace xAOD{
   typedef eFexEMRoIContainer_v1 eFexEMRoIContainer;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::eFexEMRoIContainer , 1176191655 , 1 )

#endif // XAODTRIGGER_EMTAUROICONTAINER_H
