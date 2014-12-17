// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TriggerMenuAuxContainer.h 631149 2014-11-26 12:26:18Z krasznaa $
#ifndef XAODTRIGGER_TRIGGERMENUAUXCONTAINER_H
#define XAODTRIGGER_TRIGGERMENUAUXCONTAINER_H

// Local include(s):
#include "xAODTrigger/versions/TriggerMenuAuxContainer_v1.h"

namespace xAOD {
   /// Define the latest version of the trigger menu auxiliary container class
   typedef TriggerMenuAuxContainer_v1 TriggerMenuAuxContainer;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TriggerMenuAuxContainer, 1212409402, 1 )

#endif // XAODTRIGGER_TRIGGERMENUAUXCONTAINER_H
