/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODEventShapeCnv.cxx 632142 2014-11-29 12:59:50Z krasznaa $

// System include(s):
#include <exception>

// Local include(s):
#include "xAODEventShapeCnv.h"

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

xAODEventShapeCnv::xAODEventShapeCnv( ISvcLocator* svcLoc )
   : xAODEventShapeCnvBase( svcLoc ), m_key() {

}

StatusCode xAODEventShapeCnv::createObj( IOpaqueAddress* pAddr,
                                        DataObject*& pObj ) {

   // Get the key of the object that we'll be creating:
   m_key = *( pAddr->par() + 1 );
   ATH_MSG_VERBOSE( "Key of xAOD::EventShape: " << m_key );

   // Let the base class do its thing now:
   return AthenaPoolConverter::createObj( pAddr, pObj );
}

xAOD::EventShape* xAODEventShapeCnv::createPersistent( xAOD::EventShape* trans ) {

   // Create a new object:
   xAOD::EventShape* result = new xAOD::EventShape();

   // Make it see the auxiliary store of the parent object. But not be able
   // to modify it:
   result->setStore( trans->getConstStore() );

   // Return the new object:
   return result;
}

xAOD::EventShape* xAODEventShapeCnv::createTransient() {

   // The known ID(s) for the object:
   static const pool::Guid v1_guid( "E06E8747-7C16-4BBA-B648-68F64BA70B7D" );

   // Check if we're reading the most up to date type:
   if( compareClassGuid( v1_guid ) ) {
      xAOD::EventShape* ei = poolReadObject< xAOD::EventShape >();
      setStoreLink( ei, m_key );
      return ei;
   }

   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of xAOD::EventShape found" );
   return 0;
}
