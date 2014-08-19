// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventInfoContainer_v1.h 594315 2014-04-25 17:34:40Z krasznaa $
#ifndef XAODEVENTINFO_EVENTINFOCONTAINER_V1_H
#define XAODEVENTINFO_EVENTINFOCONTAINER_V1_H

// EDM include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODEventInfo/versions/EventInfo_v1.h"

namespace xAOD {

   /// Container type for event info objects
   ///
   /// We use a container of EventInfo objects to describe the information
   /// about pileup MC events. The most important information for the users
   /// is coming from the standalone EventInfo object in the event, and in
   /// principle one should always access the pileup EventInfo objects
   /// through that main EventInfo object as well.
   ///
   typedef DataVector< EventInfo_v1 > EventInfoContainer_v1;

} // namespace xAOD

// Set up a CLID for the container:
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::EventInfoContainer_v1, 1320594112, 1 )
#endif // not XAOD_STANDALONE

#endif // XAODEVENTINFO_EVENTINFOCONTAINER_V1_H
