// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigNavigation.h 631149 2014-11-26 12:26:18Z krasznaa $
#ifndef XAODTRIGGER_TRIGNAVIGATION_H
#define XAODTRIGGER_TRIGNAVIGATION_H

// Local include(s):
#include "xAODTrigger/versions/TrigNavigation_v1.h"

namespace xAOD {
   /// Define the latest version of the trigger navigation class
   typedef TrigNavigation_v1 TrigNavigation;
}

// Set up a CLID for the type:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TrigNavigation, 182785763, 1 )

#endif // XAODTRIGGER_TRIGNAVIGATION_H
