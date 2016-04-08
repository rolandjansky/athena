// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODEVENTSHAPE_VERSIONS_EVENTSHAPEAUXINFO_V1_H
#define XAODEVENTSHAPE_VERSIONS_EVENTSHAPEAUXINFO_V1_H

// EDM include(s):
#include "xAODCore/AuxInfoBase.h"

namespace xAOD {

   /// Auxiliary information about the event shapes
   ///
   /// This object describes the "static" auxiliary information about
   /// an event shape
   ///
   /// @author P-A Delsart <delsart in2p3. fr>
   ///
   class EventShapeAuxInfo_v1 : public AuxInfoBase {

   public:
      /// Default constructor
      EventShapeAuxInfo_v1();

   }; // class EventShapeAuxInfo_v1

} // namespace xAOD

// Declare the inheritance of the class towards SG:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::EventShapeAuxInfo_v1, xAOD::AuxInfoBase );

#endif // XAODEVENTSHAPE_VERSIONS_EVENTSHAPEAUXINFO_V1_H
