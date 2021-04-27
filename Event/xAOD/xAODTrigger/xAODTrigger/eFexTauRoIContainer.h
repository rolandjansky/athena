// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGGER_EFEXTAUROICONTAINER_H
#define XAODTRIGGER_EFEXTAUROICONTAINER_H

// Local include(s):
#include "xAODTrigger/eFexTauRoI.h"
#include "xAODTrigger/versions/eFexTauRoIContainer_v1.h"

namespace xAOD{
   typedef eFexTauRoIContainer_v1 eFexTauRoIContainer;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::eFexTauRoIContainer , 1135433450 , 1 )

#endif // XAODTRIGGER_EFEXTAUROICONTAINER_H
