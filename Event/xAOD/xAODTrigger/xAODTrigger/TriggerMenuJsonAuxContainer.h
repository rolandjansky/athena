// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TriggerMenuAuxContainer.h 631149 2014-11-26 12:26:18Z krasznaa $
#ifndef XAODTRIGGER_TRIGGERMENUJSONAUXCONTAINER_H
#define XAODTRIGGER_TRIGGERMENUJSONAUXCONTAINER_H

// Local include(s):
#include "xAODTrigger/versions/TriggerMenuJsonAuxContainer_v1.h"

namespace xAOD {
   /// Define the latest version of the trigger menu auxiliary container class
   typedef TriggerMenuJsonAuxContainer_v1 TriggerMenuJsonAuxContainer;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TriggerMenuJsonAuxContainer, 373045213, 1 )

#endif // XAODTRIGGER_TRIGGERMENUJSONAUXCONTAINER_H
