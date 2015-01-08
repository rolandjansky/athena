// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventInfo.h 629232 2014-11-18 18:17:36Z krasznaa $
#ifndef XAODEVENTINFO_EVENTINFO_H
#define XAODEVENTINFO_EVENTINFO_H

// Local include(s):
#include "xAODEventInfo/versions/EventInfo_v1.h"

namespace xAOD {
   /// Definition of the latest event info version
   typedef EventInfo_v1 EventInfo;
}

// Define a CLID for the type:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::EventInfo, 45903698, 1 )

#endif // XAODEVENTINFO_EVENTINFO_H
