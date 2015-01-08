// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventInfoAuxContainer.h 629232 2014-11-18 18:17:36Z krasznaa $
#ifndef XAODEVENTINFO_EVENTINFOAUXCONTAINER_H
#define XAODEVENTINFO_EVENTINFOAUXCONTAINER_H

// Local include(s):
#include "xAODEventInfo/versions/EventInfoAuxContainer_v1.h"

namespace xAOD {
   /// Define the latest version of the auxiliary container
   typedef EventInfoAuxContainer_v1 EventInfoAuxContainer;
}

// Set up a CLID for the object:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::EventInfoAuxContainer, 1272974287, 1 )

#endif // XAODEVENTINFO_EVENTINFOAUXCONTAINER_H
