// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigNavigationAuxInfo.h 631149 2014-11-26 12:26:18Z krasznaa $
#ifndef XAODTRIGGER_TRIGNAVIGATIONAUXINFO_H
#define XAODTRIGGER_TRIGNAVIGATIONAUXINFO_H

// Local include(s):
#include "xAODTrigger/versions/TrigNavigationAuxInfo_v1.h"

namespace xAOD {
   /// Define the latest version of the trigger navigation auxiliary
   /// information class
   typedef TrigNavigationAuxInfo_v1 TrigNavigationAuxInfo;
}

// Set up a CLID for the type:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TrigNavigationAuxInfo, 80701444, 1 )

#endif // XAODTRIGGER_TRIGNAVIGATIONAUXINFO_H
