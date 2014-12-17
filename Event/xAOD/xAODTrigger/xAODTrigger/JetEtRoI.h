// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetEtRoI.h 631149 2014-11-26 12:26:18Z krasznaa $
#ifndef XAODTRIGGER_JETETROI_H
#define XAODTRIGGER_JETETROI_H

// Local include(s):
#include "xAODTrigger/versions/JetEtRoI_v1.h"

namespace xAOD{
   typedef JetEtRoI_v1 JetEtRoI;
}

// Set up a CLID for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::JetEtRoI, 96158101, 1 )

#endif // XAODTRIGGER_JETETROI_H
