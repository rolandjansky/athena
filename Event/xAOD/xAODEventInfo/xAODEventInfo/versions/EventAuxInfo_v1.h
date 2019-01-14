// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventAuxInfo_v1.h 636390 2014-12-16 21:52:18Z cranshaw $
#ifndef XAODEVENTINFO_VERSIONS_EVENTAUXINFO_V1_H
#define XAODEVENTINFO_VERSIONS_EVENTAUXINFO_V1_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <vector>
#include <string>

// EDM include(s):
#include "xAODCore/AuxInfoBase.h"
#include "CxxUtils/ConcurrentBitset.h"

namespace xAOD {

   /// Auxiliary information about the event
   ///
   /// This object describes the "static" auxiliary information about
   /// an event.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 636390 $
   /// $Date: 2014-12-16 22:52:18 +0100 (Tue, 16 Dec 2014) $
   ///
   class EventAuxInfo_v1 : public AuxInfoBase {

   public:
      /// Default constructor
      EventAuxInfo_v1();


     /**
       * @brief Return the data vector for one aux data decoration item.
       * @param auxid The identifier of the desired aux data item.
       * @param size The current size of the container (in case the data item
       *             does not already exist).
       * @param capacity The current capacity of the container (in case
       *                 the data item does not already exist).
       */
      virtual void* getDecoration (SG::auxid_t auxid,
                                   size_t size,
                                   size_t capacity) override;


     /**
      * @brief Test if a particular variable is tagged as a decoration.
      * @param auxid The identifier of the desired aux data item.
      */
     virtual bool isDecoration (auxid_t auxid) const override;


     /**
      * @brief Lock a decoration.
      * @param auxid Identifier of the decoration to lock.
      */
     virtual void lockDecoration (SG::auxid_t auxid) override;


     /**
      * @brief Called after one of these objects is read.
      *        Locks any detector flag words that appear to have already
      *        been set.
      */
     void toTransient();


   private:
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

      /// @name Pileup information
      /// @{
      float actualInteractionsPerCrossing;
      float averageInteractionsPerCrossing;
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


      /// Keep track of the event status flags.
      /// The set bits here correspond to the auxids of all unlocked
      /// detector flag words.  This is not persistent,
      /// but is filled in the constructor and toTransient.
      CxxUtils::ConcurrentBitset m_decorFlags;

   }; // class EventAuxInfo_v1

} // namespace xAOD

// Declare the inheritance of the type:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::EventAuxInfo_v1, xAOD::AuxInfoBase );

#endif // XAODEVENTINFO_VERSIONS_EVENTAUXINFO_V1_H
