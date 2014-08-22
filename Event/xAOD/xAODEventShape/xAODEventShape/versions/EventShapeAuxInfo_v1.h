// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODEVENTSHAPE_VERSIONS_EVENTAUXSHAPE_V1_H
#define XAODEVENTSHAPE_VERSIONS_EVENTAUXSHAPE_V1_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <vector>
#include <string>

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

   private:
      /// @name Basic event information
      /// @{
      // uint32_t runNumber;
      // unsigned long long eventNumber;
      // uint32_t lumiBlock;
      // uint32_t timeStamp;
      // uint32_t timeStampNSOffset;
      // uint32_t bcid;
      // uint32_t detectorMask0;
      // uint32_t detectorMask1;
      // /// @}


   }; // class EventShapeAuxInfo_v1

} // namespace xAOD

// Set up a CLID for the object:
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::EventShapeAuxInfo_v1, 129946083, 1 )
#include "SGTools/BaseInfo.h"
SG_BASE( xAOD::EventShapeAuxInfo_v1, xAOD::AuxInfoBase );
#endif // not XAOD_STANDALONE

#endif // XAODEVENTSHAPE_VERSIONS_EVENTAUXSHAPE_V1_H
