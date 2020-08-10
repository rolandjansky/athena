// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: eFexEMRoIAuxContainer.h 2019-07-01 13:04:25Z watsona $
#ifndef XAODTRIGGER_EFEXEMROIAUXCONTAINER_H
#define XAODTRIGGER_EFEXEMROIAUXCONTAINER_H

// Local include(s):
#include "xAODTrigger/versions/eFexEMRoIAuxContainer_v1.h"

namespace xAOD{
   typedef eFexEMRoIAuxContainer_v1 eFexEMRoIAuxContainer;
}

// Set up a CLID for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::eFexEMRoIAuxContainer , 1081052998 , 1 )

#endif // XAODTRIGGER_EFEXEMROIAUXCONTAINER_H
