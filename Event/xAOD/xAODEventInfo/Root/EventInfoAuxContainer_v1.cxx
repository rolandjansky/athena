/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventInfoAuxContainer_v1.cxx 636390 2014-12-16 21:52:18Z cranshaw $

// Local include(s):
#include "xAODEventInfo/versions/EventInfoAuxContainer_v1.h"

namespace xAOD {

   EventInfoAuxContainer_v1::EventInfoAuxContainer_v1()
      : AuxContainerBase(),
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

      // MC information:
      AUX_VARIABLE( mcChannelNumber );
      AUX_VARIABLE( mcEventNumber );
      AUX_VARIABLE( mcEventWeights );
      
      // Pileup information:
      AUX_VARIABLE( pileUpMixtureIDLowBits );
      AUX_VARIABLE( pileUpMixtureIDHighBits );
   }

   /**
    * @brief Return the data vector for one aux data decoration item.
    * @param auxid The identifier of the desired aux data item.
    * @param size The current size of the container (in case the data item
    *             does not already exist).
    * @param capacity The current capacity of the container (in case
    *                 the data item does not already exist).
    */
   void* EventInfoAuxContainer_v1::getDecoration (SG::auxid_t auxid,
                                                  size_t size,
                                                  size_t capacity)
   {
     if (m_decorFlags.test (auxid)) {
       return AuxContainerBase::getData (auxid, size, capacity);
     }

     return AuxContainerBase::getDecoration (auxid, size, capacity);
   }


   /**
    * @brief Test if a particular variable is tagged as a decoration.
    * @param auxid The identifier of the desired aux data item.
    */
   bool EventInfoAuxContainer_v1::isDecoration (SG::auxid_t auxid) const
   {
     if (m_decorFlags.test (auxid)) {
       return true;
     }

     return AuxContainerBase::isDecoration (auxid);
   }


   /**
    * @brief Lock a decoration.
    * @param auxid Identifier of the decoration to lock.
    */
   void EventInfoAuxContainer_v1::lockDecoration (SG::auxid_t auxid)
   {
     if (m_decorFlags.test (auxid)) {
       m_decorFlags.reset (auxid);
       return;
     }

     AuxContainerBase::lockDecoration (auxid);
   }


   /**
    * @brief Called after one of these objects is read.
    *        Locks any detector flag words that appear to have already
    *        been set.
    */
   void EventInfoAuxContainer_v1::toTransient()
   {
     /// List of all detector flag aux IDs.
     static const std::vector<SG::auxid_t> flagIds (m_decorFlags.begin(),
                                                    m_decorFlags.end());

     // Check each detector flag.  If it has been set to something,
     // then lock this flag.
     // This check may be too strict; we'll have to see.
     size_t sz = size();
     for (SG::auxid_t id : flagIds) {
       const uint32_t* flags = reinterpret_cast<const uint32_t*>(getData (id, sz, sz));
       for (size_t i = 0; i < sz; i++) {
         if (flags[i] != 0) {
           m_decorFlags.reset (id);
           break;
         }
       }
     }
   }


} // namespace xAOD
