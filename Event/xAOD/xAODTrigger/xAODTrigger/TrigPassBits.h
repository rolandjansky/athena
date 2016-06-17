// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigPassBits.h 704127 2015-10-28 14:18:55Z krasznaa $
#ifndef XAODTRIGGER_TRIGPASSBITS_H
#define XAODTRIGGER_TRIGPASSBITS_H

// Local include(s):
#include "xAODTrigger/versions/TrigPassBits_v1.h"

namespace xAOD {
   /// Define the latest version of the trigger pass bits class
   typedef TrigPassBits_v1 TrigPassBits;
}

// Set up a CLID for the type:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TrigPassBits, 189822171, 1 )

#endif // XAODTRIGGER_TRIGPASSBITS_H
