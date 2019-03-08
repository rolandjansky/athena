// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventFormat.h 629191 2014-11-18 17:01:16Z krasznaa $
#ifndef XAODEVENTFORMAT_EVENTFORMAT_H
#define XAODEVENTFORMAT_EVENTFORMAT_H

// Local include(s):
#include "xAODEventFormat/versions/EventFormat_v1.h"

namespace xAOD {
   /// Definition of the current event format version
   typedef EventFormat_v1 EventFormat;
}

// Declare a CLID for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::EventFormat, 243004407, 1 )
#ifndef XAOD_STANDALONE
#include "AthenaKernel/MetaCont.h"
CLASS_DEF( MetaCont<xAOD::EventFormat> , 35480469 , 1 )
#include "SGTools/BaseInfo.h"
SG_BASE( MetaCont<xAOD::EventFormat>, MetaContBase );
#endif // not XAOD_STANDALONE

#endif // XAODEVENTFORMAT_EVENTFORMAT_H
