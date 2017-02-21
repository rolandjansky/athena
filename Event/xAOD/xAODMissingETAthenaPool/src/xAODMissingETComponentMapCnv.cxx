/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 
// $Id: xAODMissingETComponentMapCnv.cxx 795699 2017-02-05 23:26:05Z khoo $

// System include(s):
#include <exception>

// Local include(s):
#include "xAODMissingETComponentMapCnv.h"

namespace {

  /// Helper function setting up the container's link to its auxiliary store
  void setStoreLink( SG::AuxVectorBase* cont, const std::string& key ) {

    // The link to set up:
    DataLink< SG::IConstAuxStore > link( key + "Aux." );

    // Give it to the container:
    cont->setStore( link );

    return;
  }

} // private namespace

xAODMissingETComponentMapCnv::xAODMissingETComponentMapCnv( ISvcLocator* svcLoc )
  : xAODMissingETComponentMapCnvBase( svcLoc ) {

}

/**
 * This function needs to be re-implemented in order to figure out the StoreGate
 * key of the container that's being created. After that's done, it lets the
 * base class do its normal task.
 */
StatusCode xAODMissingETComponentMapCnv::createObj( IOpaqueAddress* pAddr,
						    DataObject*& pObj ) {

  // Get the key of the container that we'll be creating:
  m_key = *( pAddr->par() + 1 );
  ATH_MSG_VERBOSE( "Key of xAOD::MissingETComponentMap: " << m_key );

  // Let the base class do its thing now:
  return AthenaPoolConverter::createObj( pAddr, pObj );
}

xAOD::MissingETComponentMap*
xAODMissingETComponentMapCnv::createPersistent( xAOD::MissingETComponentMap* trans ) {

  // Create a view copy of the container:
  xAOD::MissingETComponentMap* result =
    new xAOD::MissingETComponentMap( trans->begin(), trans->end(), SG::VIEW_ELEMENTS );

  // // Prepare the objects to be written out:
  // xAOD::MissingETComponentMap::iterator itr = result->begin();
  // xAOD::MissingETComponentMap::iterator end = result->end();
  // for( ; itr != end; ++itr ) {
  //   toPersistent( *itr );
  // }

  // Return the new container:
  return result;
}

xAOD::MissingETComponentMap* xAODMissingETComponentMapCnv::createTransient() {

  // The known ID(s) for this container:
  static pool::Guid v1_guid( "994D9D32-820F-47B1-A54B-37C15CD0FD1E" );

  // Check if we're reading the most up to date type:
  if( compareClassGuid( v1_guid ) ) {
    xAOD::MissingETComponentMap* c = poolReadObject< xAOD::MissingETComponentMap >();

    setStoreLink( c, m_key );
    return c;
  }

  // If we didn't recognise the ID, let's complain:
  throw std::runtime_error( "Unsupported version of "
			    "xAOD::MissingETComponentMap found" );
  return 0;
}

// void xAODMissingETComponentMapCnv::toPersistent( xAOD::MissingETComponent* comp ) const {

//   // Tell the object to prepare all its smart pointers for persistification:
//   comp->updateLinks();

//   return;
// }
