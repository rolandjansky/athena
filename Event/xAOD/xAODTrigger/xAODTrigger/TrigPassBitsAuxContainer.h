// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigPassBitsAuxContainer.h 702738 2015-10-23 16:00:51Z krasznaa $
#ifndef XAODTRIGGER_TRIGPASSBITSAUXCONTAINER_H
#define XAODTRIGGER_TRIGPASSBITSAUXCONTAINER_H

// Local include(s):
#include "xAODTrigger/versions/TrigPassBitsAuxContainer_v1.h"

namespace xAOD {
   /// Define the latest version of the class
   typedef TrigPassBitsAuxContainer_v1 TrigPassBitsAuxContainer;
}

// Set up a CLID for the type:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TrigPassBitsAuxContainer, 1232503388, 1 )

#endif // XAODTRIGGER_TRIGPASSBITSAUXCONTAINER_H
