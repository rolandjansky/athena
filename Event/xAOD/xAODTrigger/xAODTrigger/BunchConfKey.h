// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BunchConfKey.h 630856 2014-11-25 11:22:49Z gwatts $
#ifndef XAODTRIGGER_BUNCHCONFKEY_H
#define XAODTRIGGER_BUNCHCONFKEY_H

// Local include(s):
#include "xAODTrigger/versions/BunchConfKey_v1.h"

namespace xAOD {
   /// Declare the current version of the bunch configuration key type
   typedef BunchConfKey_v1 BunchConfKey;
}

// Declare the CLID for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::BunchConfKey, 27810374, 1 )

#endif // XAODTRIGGER_BUNCHCONFKEY_H
