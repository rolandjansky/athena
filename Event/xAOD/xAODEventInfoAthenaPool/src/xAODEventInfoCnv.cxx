/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODEventInfoCnv.cxx 594317 2014-04-25 17:36:58Z krasznaa $

// System include(s):
#include <exception>

// Local include(s):
#include "xAODEventInfoCnv.h"

namespace {

   /// Helper function to set up the object's link to its auxiliary store
   void setStoreLink( SG::AuxElement* el, const std::string& key ) {

      // The link to set up:
      DataLink< SG::IConstAuxStore > link( key + "Aux." );

      // Give it to the object:
      el->setStore( link );

      return;
   }

} // private namespace

xAODEventInfoCnv::xAODEventInfoCnv( ISvcLocator* svcLoc )
   : xAODEventInfoCnvBase( svcLoc ), m_key() {

}

StatusCode xAODEventInfoCnv::createObj( IOpaqueAddress* pAddr,
                                        DataObject*& pObj ) {

   // Get the key of the object that we'll be creating:
   m_key = *( pAddr->par() + 1 );
   ATH_MSG_VERBOSE( "Key of xAOD::EventInfo: " << m_key );

   // Let the base class do its thing now:
   return AthenaPoolConverter::createObj( pAddr, pObj );
}

xAOD::EventInfo* xAODEventInfoCnv::createPersistent( xAOD::EventInfo* trans ) {

   // Prepare the original object for persistification:
   trans->toPersistent();

   // Create a new object:
   xAOD::EventInfo* result = new xAOD::EventInfo();

   // Make it see the auxiliary store of the parent object. But not be able
   // to modify it:
   result->setStore( trans->getConstStore() );

   // Return the new object:
   return result;
}

xAOD::EventInfo* xAODEventInfoCnv::createTransient() {

   // The known ID(s) for the object:
   static const pool::Guid v1_guid( "AE8BED6D-1D41-4CAF-994B-42613FC91A0A" );

   // Check if we're reading the most up to date type:
   if( compareClassGuid( v1_guid ) ) {
      xAOD::EventInfo* ei = poolReadObject< xAOD::EventInfo >();
      setStoreLink( ei, m_key );
      return ei;
   }

   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of xAOD::EventInfo found" );
   return 0;
}
