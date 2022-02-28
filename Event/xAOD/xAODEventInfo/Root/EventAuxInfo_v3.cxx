/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "xAODEventInfo/versions/EventAuxInfo_v3.h"

namespace xAOD {

   EventAuxInfo_v3::EventAuxInfo_v3()
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

      // Detector flags:
#define DET_FLAG(VAR)                                                   \
      AUX_VARIABLE( VAR, SG::AuxTypeRegistry::Flags::Atomic );          \
      do {                                                              \
         static const auxid_t auxid =                                   \
            getAuxID( #VAR, VAR,                                        \
                      SG::AuxTypeRegistry::Flags::Atomic );             \
         m_decorFlags.insert( auxid );                                  \
      } while( false )

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
   void* EventAuxInfo_v3::getDecoration (SG::auxid_t auxid,
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
   bool EventAuxInfo_v3::isDecoration (SG::auxid_t auxid) const
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
   void EventAuxInfo_v3::lockDecoration (SG::auxid_t auxid)
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
   void EventAuxInfo_v3::toTransient()
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
