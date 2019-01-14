/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventAuxInfo_v1.cxx 657675 2015-03-30 01:36:11Z ssnyder $

// Local include(s):
#include "xAODEventInfo/versions/EventAuxInfo_v1.h"

namespace xAOD {

   EventAuxInfo_v1::EventAuxInfo_v1()
     : AuxInfoBase(),
       runNumber(0),
       eventNumber(0),
       lumiBlock(0),
       timeStamp(0),
       timeStampNSOffset(0),
       bcid(0),
       detectorMask0(0),
       detectorMask1(0),
       detectorMask2(0),
       detectorMask3(0),
       eventTypeBitmask(0),
       statusElement(0),
       extendedLevel1ID(0),
       level1TriggerType(0),
       actualInteractionsPerCrossing(0),
       averageInteractionsPerCrossing(0),
       pixelFlags(0),
       sctFlags(0),
       trtFlags(0),
       larFlags(0),
       tileFlags(0),
       muonFlags(0),
       forwardDetFlags(0),
       coreFlags(0),
       backgroundFlags(0),
       lumiFlags(0),
       beamPosX(0),
       beamPosY(0),
       beamPosZ(0),
       beamPosSigmaX(0),
       beamPosSigmaY(0),
       beamPosSigmaZ(0),
       beamPosSigmaXY(0),
       beamTiltXZ(0),
       beamTiltYZ(0),
       beamStatus(0),
       m_decorFlags (SG::auxid_set_size_hint)
   {

      // Basic event information:
      AUX_VARIABLE( runNumber );
      AUX_VARIABLE( eventNumber );
      AUX_VARIABLE( lumiBlock );
      AUX_VARIABLE( timeStamp );
      AUX_VARIABLE( timeStampNSOffset );
      AUX_VARIABLE( bcid );
      AUX_VARIABLE( detectorMask0 );
      AUX_VARIABLE( detectorMask1 );
      AUX_VARIABLE( detectorMask2 );
      AUX_VARIABLE( detectorMask3 );

      // Event type information:
      AUX_VARIABLE( detDescrTags );
      AUX_VARIABLE( eventTypeBitmask );

      // Trigger related information:
      AUX_VARIABLE( statusElement );
      AUX_VARIABLE( extendedLevel1ID );
      AUX_VARIABLE( level1TriggerType );
      AUX_VARIABLE( streamTagNames );
      AUX_VARIABLE( streamTagTypes );
      AUX_VARIABLE( streamTagObeysLumiblock );

      // Pileup information:
      AUX_VARIABLE( actualInteractionsPerCrossing );
      AUX_VARIABLE( averageInteractionsPerCrossing );

      // Beam spot information:
      AUX_VARIABLE( beamPosX );
      AUX_VARIABLE( beamPosY );
      AUX_VARIABLE( beamPosZ );
      AUX_VARIABLE( beamPosSigmaX );
      AUX_VARIABLE( beamPosSigmaY );
      AUX_VARIABLE( beamPosSigmaZ );
      AUX_VARIABLE( beamPosSigmaXY );
      AUX_VARIABLE( beamTiltXZ );
      AUX_VARIABLE( beamTiltYZ );
      AUX_VARIABLE( beamStatus );

      // Detector flags:
#define DET_FLAG(VAR) \
 m_decorFlags.insert (AUX_VARIABLE( VAR, SG::AuxTypeRegistry::Flags::Atomic ))
      
      DET_FLAG( pixelFlags );
      DET_FLAG( sctFlags );
      DET_FLAG( trtFlags );
      DET_FLAG( larFlags );
      DET_FLAG( tileFlags );
      DET_FLAG( muonFlags );
      DET_FLAG( forwardDetFlags );
      DET_FLAG( coreFlags );
      DET_FLAG( backgroundFlags );
      DET_FLAG( lumiFlags );
#undef DET_FLAG
   }


   /**
    * @brief Return the data vector for one aux data decoration item.
    * @param auxid The identifier of the desired aux data item.
    * @param size The current size of the container (in case the data item
    *             does not already exist).
    * @param capacity The current capacity of the container (in case
    *                 the data item does not already exist).
    */
   void* EventAuxInfo_v1::getDecoration (SG::auxid_t auxid,
                                         size_t size,
                                         size_t capacity)
   {
     if (m_decorFlags.test (auxid)) {
       return AuxInfoBase::getData (auxid, size, capacity);
     }

     return AuxInfoBase::getDecoration (auxid, size, capacity);
   }


   /**
    * @brief Test if a particular variable is tagged as a decoration.
    * @param auxid The identifier of the desired aux data item.
    */
   bool EventAuxInfo_v1::isDecoration (SG::auxid_t auxid) const
   {
     if (m_decorFlags.test (auxid)) {
       return true;
     }

     return AuxInfoBase::isDecoration (auxid);
   }


  /**
    * @brief Lock a decoration.
    * @param auxid Identifier of the decoration to lock.
    */
   void EventAuxInfo_v1::lockDecoration (SG::auxid_t auxid)
   {
     if (m_decorFlags.test (auxid)) {
       m_decorFlags.reset (auxid);
       return;
     }

     AuxInfoBase::lockDecoration (auxid);
   }


   /**
    * @brief Called after one of these objects is read.
    *        Locks any detector flag words that appear to have already
    *        been set.
    */
   void EventAuxInfo_v1::toTransient()
   {
     /// List of all detector flag aux IDs.
     static const std::vector<SG::auxid_t> flagIds (m_decorFlags.begin(),
                                                    m_decorFlags.end());

     // Check each detector flag.  If it has been set to something,
     // then lock this flag.
     // This check may be too strict; we'll have to see.
     for (SG::auxid_t id : flagIds) {
       if (*reinterpret_cast<const uint32_t*>(getData (id)) != 0) {
         m_decorFlags.reset (id);
       }
     }
   }


} // namespace xAOD
