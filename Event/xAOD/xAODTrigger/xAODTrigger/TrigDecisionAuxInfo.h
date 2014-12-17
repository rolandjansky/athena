// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigDecisionAuxInfo.h 631149 2014-11-26 12:26:18Z krasznaa $
#ifndef XAODTRIGGER_TRIGDECISIONAUXINFO_H
#define XAODTRIGGER_TRIGDECISIONAUXINFO_H

// Local include(s):
#include "xAODTrigger/versions/TrigDecisionAuxInfo_v1.h"

namespace xAOD {
   /// Define the latest version of the trigger decision auxiliary
   /// information class
   typedef TrigDecisionAuxInfo_v1 TrigDecisionAuxInfo;
}

// Set up a CLID for the type:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TrigDecisionAuxInfo, 44251926, 1 )

#endif // XAODTRIGGER_TRIGDECISIONAUXINFO_H
