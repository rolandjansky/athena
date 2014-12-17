// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BunchConfContainer.h 630856 2014-11-25 11:22:49Z gwatts $
#ifndef XAODTRIGGER_BUNCHCONFCONTAINER_H
#define XAODTRIGGER_BUNCHCONFCONTAINER_H

// Local include(s):
#include "xAODTrigger/versions/BunchConfContainer_v1.h"
#include "xAODTrigger/BunchConf.h"

namespace xAOD {
   /// Define the latest version of the bunch configuration container class
   typedef BunchConfContainer_v1 BunchConfContainer;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::BunchConfContainer, 1244568624, 1 )

#endif // XAODTRIGGER_BUNCHCONFCONTAINER_H
