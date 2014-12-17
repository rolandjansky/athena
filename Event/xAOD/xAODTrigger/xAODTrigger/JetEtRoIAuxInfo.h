// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetEtRoIAuxInfo.h 631149 2014-11-26 12:26:18Z krasznaa $
#ifndef XAODTRIGGER_JETETROIAUXINFO_H
#define XAODTRIGGER_JETETROIAUXINFO_H

// Local include(s):
#include "xAODTrigger/versions/JetEtRoIAuxInfo_v1.h"

namespace xAOD{
   /// Declare the most recent version of the jet-ET auxiliary info
   typedef JetEtRoIAuxInfo_v1 JetEtRoIAuxInfo;
}

// Set up a CLID for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::JetEtRoIAuxInfo, 261989842, 1 )

#endif // XAODTRIGGER_JETETROIAUXINFO_H
