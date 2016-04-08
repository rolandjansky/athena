// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: HIEventShapeContainer_v1.h 693427 2015-09-07 11:45:19Z krasznaa $
#ifndef XAODHIEVENT_HIEVENTSHAPECONTAINER_V1_H
#define XAODHIEVENT_HIEVENTSHAPECONTAINER_V1_H

// EDM include(s):
#include "AthContainers/DataVector.h"

//#include <vector>

//#include "xAODCore/AuxContainerBase.h"

// #include "AthLinks/ElementLink.h"
// Local include(s):
#include "xAODHIEvent/versions/HIEventShape_v1.h"

namespace xAOD {

   /// Container type for event info objects
   ///
   /// We use a container of HIEventShape objects to describe the information
   /// about pileup MC events. The most important information for the users
   /// is coming from the standalone HIEventShape object in the event, and in
   /// principle one should always access the pileup HIEventShape objects
   /// through that main HIEventShape object as well.
   ///
  typedef DataVector< HIEventShape_v1 > HIEventShapeContainer_v1;


} // namespace xAOD


#endif // XAODEVENTINFO_EVENTINFOCONTAINER_V1_H
