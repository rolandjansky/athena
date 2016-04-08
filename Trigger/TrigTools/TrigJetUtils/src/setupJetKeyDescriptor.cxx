/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: setupJetKeyDescriptor.cxx 588111 2014-03-18 09:02:18Z krasznaa $

// Gaudi/Athena include(s):
#include "StoreGate/StoreGateSvc.h"

// EDM include(s):
#include "JetEvent/JetKeyDescriptor.h"

// Local include(s):
#include "setupJetKeyDescriptor.h"

namespace TrigJetUtils {

   StatusCode setupJetKeyDescriptor( StoreGateSvc* sg, MsgStream& log ) {

      ///////////////////////////////////////////
      // make sure, we have a JetKeyDescriptor //
      // with key "JetKeyMap" in StoreGate     //
      ///////////////////////////////////////////
      static const std::string storeKey = "JetKeyMap";
 
      if( log.level() <= MSG::DEBUG ) {
         log << MSG::DEBUG
             << "Checking if StoreGate already contains a JetKeyMap ..."
             << endreq;
      }

      const JetKeyDescriptor* jetKeyDescrConst = 0;
      const bool retrieved = sg->contains< JetKeyDescriptor >( storeKey );
      if( retrieved ) {

         if( log.level() <= MSG::VERBOSE ) {
            log << MSG::VERBOSE
                << "StoreGate contains a JetKeyDescriptor with key <"
                << storeKey << "> ! Trying to retrieve it " << endreq;
         }

         if( sg->retrieve( jetKeyDescrConst, storeKey ).isFailure() ) {
            log << MSG::FATAL << "Couldn't retrieve JetKeyDescriptor with "
                << "key: " << storeKey << endreq;
            return StatusCode::FAILURE;
         }

         if( log.level() <= MSG::VERBOSE ) {
            log << MSG::VERBOSE << "printing" << endreq;
            JetKeyDescriptorInstance::instance()->printOut( log );
            log << MSG::VERBOSE << "PRINT >" << jetKeyDescrConst << "<"
                << endreq;
         }

      } else {

         // no JetKeyDescriptor, probably because we run on RDO
         //  and didn't make any jets yet ! Create a new one
         //  and tell it to connect to the instance...
         JetKeyDescriptor* jetKeyDescr =
            JetKeyDescriptorInstance::instance()->getKeyStore();

         if( log.level() <= MSG::DEBUG ) {
            log << MSG::DEBUG << "Trying to record JetKeyDescriptor wit key <"
                << storeKey << "> into StoreGate !" << endreq;
         }
         if( sg->record( jetKeyDescr, storeKey ).isFailure() ) {
            log << MSG::WARNING << "Error recording JetKeyMap with key <"
                << storeKey << "> into StoreGate !"
                << " Giving up!" << endreq;
         }
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace TrigJetUtils
