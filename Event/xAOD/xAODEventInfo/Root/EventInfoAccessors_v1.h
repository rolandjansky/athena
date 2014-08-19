// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventInfoAccessors_v1.h 579930 2014-01-24 10:36:54Z krasznaa $
#ifndef XAODEVENTINFO_EVENTINFOACCESSORS_V1_H
#define XAODEVENTINFO_EVENTINFOACCESSORS_V1_H

// Local include(s):
#include "xAODEventInfo/versions/EventInfo_v1.h"

namespace xAOD {

   /// Helper function for accessing event flag variables
   ///
   /// This function manages the Accessor objects that the code uses to
   /// get/set detector flags.
   ///
   /// @param subDet The sub-detector for which the flag accessor should be
   ///               returned
   /// @returns A pointer to the accessor object if successful, or a null
   ///          pointer if the sub-detector is unknown
   ///
   EventInfo_v1::Accessor< uint32_t >*
   eventFlagsAccessorsV1( EventInfo_v1::EventFlagSubDet subDet );

} // namespace xAOD

#endif // XAODEVENTINFO_EVENTINFOACCESSORS_V1_H
