// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TriggerMenuContainer.h 630856 2014-11-25 11:22:49Z gwatts $
#ifndef XAODTRIGGER_TRIGGERMENUCONTAINER_H
#define XAODTRIGGER_TRIGGERMENUCONTAINER_H

// Local include(s):
#include "xAODTrigger/versions/TriggerMenuContainer_v1.h"
#include "xAODTrigger/TriggerMenu.h"

namespace xAOD {
   /// Define the latest version of the trigger menu container class
   typedef TriggerMenuContainer_v1 TriggerMenuContainer;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TriggerMenuContainer, 1107011239, 1 )

#endif // XAODTRIGGER_TRIGGERMENUCONTAINER_H
