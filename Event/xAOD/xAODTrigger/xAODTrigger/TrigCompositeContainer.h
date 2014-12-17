// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigCompositeContainer.h 631149 2014-11-26 12:26:18Z krasznaa $
#ifndef XAODTRIGGER_TRIGCOMPOSITECONTAINER_H
#define XAODTRIGGER_TRIGCOMPOSITECONTAINER_H

// Local include(s):
#include "xAODTrigger/TrigComposite.h"
#include "xAODTrigger/versions/TrigCompositeContainer_v1.h"

namespace xAOD{
   /// Declare the latest version of the container
   typedef TrigCompositeContainer_v1 TrigCompositeContainer;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TrigCompositeContainer, 1333228823, 1 )

#endif // XAODTRIGGER_TRIGCOMPOSITECONTAINER_H
