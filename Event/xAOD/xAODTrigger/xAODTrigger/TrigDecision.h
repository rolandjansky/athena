// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigDecision.h 631149 2014-11-26 12:26:18Z krasznaa $
#ifndef XAODTRIGGER_TRIGDECISION_H
#define XAODTRIGGER_TRIGDECISION_H

// Local include(s):
#include "xAODTrigger/versions/TrigDecision_v1.h"

namespace xAOD {
   /// Define the latest version of the trigger decision class
   typedef TrigDecision_v1 TrigDecision;
}

// Set up a CLID for the type:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TrigDecision, 92199377, 1 )

#endif // XAODTRIGGER_TRIGDECISION_H
