// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventInfoContainer.h 629232 2014-11-18 18:17:36Z krasznaa $
#ifndef XAODEVENTINFO_EVENTINFOCONTAINER_H
#define XAODEVENTINFO_EVENTINFOCONTAINER_H

// Local include(s):
#include "xAODEventInfo/versions/EventInfoContainer_v1.h"
#include "xAODEventInfo/EventInfo.h"

namespace xAOD {
   /// Define the latest version of the container
   typedef EventInfoContainer_v1 EventInfoContainer;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::EventInfoContainer, 1320594112, 1 )

#endif // XAODEVENTINFO_EVENTINFOCONTAINER_H
