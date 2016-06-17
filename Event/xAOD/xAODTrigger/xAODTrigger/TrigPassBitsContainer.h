// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigPassBitsContainer.h 702738 2015-10-23 16:00:51Z krasznaa $
#ifndef XAODTRIGGER_TRIGPASSBITSCONTAINER_H
#define XAODTRIGGER_TRIGPASSBITSCONTAINER_H

// Local include(s):
#include "xAODTrigger/versions/TrigPassBitsContainer_v1.h"
#include "xAODTrigger/TrigPassBits.h"

namespace xAOD {
   /// Define the latest version of the trig pass bits container class
   typedef TrigPassBitsContainer_v1 TrigPassBitsContainer;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TrigPassBitsContainer, 1118282457, 1 )

#endif // XAODTRIGGER_TRIGPASSBITSCONTAINER_H
