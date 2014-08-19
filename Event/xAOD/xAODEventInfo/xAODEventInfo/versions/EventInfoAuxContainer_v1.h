// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventInfoAuxContainer_v1.h 594315 2014-04-25 17:34:40Z krasznaa $
#ifndef XAODEVENTINFO_VERSIONS_EVENTINFOAUXCONTAINER_V1_H
#define XAODEVENTINFO_VERSIONS_EVENTINFOAUXCONTAINER_V1_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <vector>
#include <string>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD {

   /// Auxiliary information about the pileup events
   ///
   /// This object describes the "static" auxiliary information about
   /// all the pileup events.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 594315 $
   /// $Date: 2014-04-25 19:34:40 +0200 (Fri, 25 Apr 2014) $
   ///
   class EventInfoAuxContainer_v1 : public AuxContainerBase {

   public:
      /// Default constructor
      EventInfoAuxContainer_v1();

   private:
      /// @name Basic event information
      /// @{
      std::vector< uint32_t > runNumber;
      std::vector< unsigned long long > eventNumber;
      std::vector< uint32_t > lumiBlock;
      std::vector< uint32_t > timeStamp;
      std::vector< uint32_t > timeStampNSOffset;
      std::vector< uint32_t > bcid;
      std::vector< uint32_t > detectorMask0;
      std::vector< uint32_t > detectorMask1;
      /// @}

      /// @name Event type information
      /// @{
      std::vector< std::vector< std::pair< std::string, std::string > > >
         detDescrTags;
      std::vector< uint32_t > eventTypeBitmask;
      /// @}

      /// @name Detector flags
      /// @{
      std::vector< uint32_t > pixelFlags;
      std::vector< uint32_t > sctFlags;
      std::vector< uint32_t > trtFlags;
      std::vector< uint32_t > larFlags;
      std::vector< uint32_t > tileFlags;
      std::vector< uint32_t > muonFlags;
      std::vector< uint32_t > forwardDetFlags;
      std::vector< uint32_t > coreFlags;
      std::vector< uint32_t > backgroundFlags;
      std::vector< uint32_t > lumiFlags;
      /// @}

      /// @name Monte-Carlo information
      /// @{
      std::vector< uint32_t > mcChannelNumber;
      std::vector< unsigned long long > mcEventNumber;
      std::vector< std::vector< float > > mcEventWeights;
      /// @}

   }; // class EventInfoAuxContainer_v1

} // namespace xAOD

// Set up a CLID for the object:
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::EventInfoAuxContainer_v1, 1272974287, 1 )
#include "SGTools/BaseInfo.h"
SG_BASE( xAOD::EventInfoAuxContainer_v1, xAOD::AuxContainerBase );
#endif // not XAOD_STANDALONE

#endif // XAODEVENTINFO_VERSIONS_EVENTINFOAUXCONTAINER_V1_H
