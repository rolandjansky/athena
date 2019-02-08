// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigCompositeAuxContainer.h 631149 2014-11-26 12:26:18Z krasznaa $
#ifndef XAODTRIGGER_TRIGCOMPOSITEAUXCONTAINER_H
#define XAODTRIGGER_TRIGCOMPOSITEAUXCONTAINER_H

// Local include(s):
#include "xAODTrigger/versions/TrigCompositeAuxContainer_v2.h"

namespace xAOD{
   /// Declare the latest version of the container
   typedef TrigCompositeAuxContainer_v2 TrigCompositeAuxContainer;
}

namespace TrigCompositeUtils{ 
   typedef xAOD::TrigCompositeAuxContainer DecisionAuxContainer;
}

// Set up a CLID for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TrigCompositeAuxContainer, 1175586382, 1 )

#endif // XAODTRIGGER_TRIGCOMPOSITEAUXCONTAINER_H
