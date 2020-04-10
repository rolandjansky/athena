// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODEVENTINFO_VERSIONS_EVENTAUXINFO_V2_H
#define XAODEVENTINFO_VERSIONS_EVENTAUXINFO_V2_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <vector>
#include <string>

// EDM include(s):
#include "xAODCore/AuxInfoBase.h"

namespace xAOD {

   /// Auxiliary information about the event
   ///
   /// This object describes the "static" auxiliary information about
   /// an event.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   class EventAuxInfo_v2 : public AuxInfoBase {

   public:
      /// Default constructor
      EventAuxInfo_v2();

      /// @name Basic event information
      /// @{
      uint32_t runNumber;
      unsigned long long eventNumber;
      uint32_t lumiBlock;
      uint32_t timeStamp;
      uint32_t timeStampNSOffset;
      uint32_t bcid;
      uint32_t detectorMask0;
      uint32_t detectorMask1;
      uint32_t detectorMask2;
      uint32_t detectorMask3;
      /// @}

      /// @name Event type information
      /// @{
      std::vector< std::pair< std::string, std::string > > detDescrTags;
      uint32_t eventTypeBitmask;
      /// @}

      /// @name Trigger related information
      /// @{
      uint32_t statusElement;
      uint32_t extendedLevel1ID;
      uint16_t level1TriggerType;
      std::vector< std::string > streamTagNames;
      std::vector< std::string > streamTagTypes;
      std::vector< char > streamTagObeysLumiblock;
      /// @}

      /// @name Detector flags
      /// @{
      uint32_t pixelFlags;
      uint32_t sctFlags;
      uint32_t trtFlags;
      uint32_t larFlags;
      uint32_t tileFlags;
      uint32_t muonFlags;
      uint32_t forwardDetFlags;
      uint32_t coreFlags;
      uint32_t backgroundFlags;
      uint32_t lumiFlags;
      /// @}

      /// @name Beam spot information
      /// @{
      float beamPosX;
      float beamPosY;
      float beamPosZ;
      float beamPosSigmaX;
      float beamPosSigmaY;
      float beamPosSigmaZ;
      float beamPosSigmaXY;
      float beamTiltXZ;
      float beamTiltYZ;
      uint32_t beamStatus;
      /// @}

   }; // class EventAuxInfo_v2

} // namespace xAOD

// Declare the inheritance of the type:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::EventAuxInfo_v2, xAOD::AuxInfoBase );

#endif // XAODEVENTINFO_VERSIONS_EVENTAUXINFO_V2_H
