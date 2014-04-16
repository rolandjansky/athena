/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 
// $Id: xAODMissingETComponentMapCnv.cxx 586838 2014-03-08 15:10:34Z khoo $

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

  // Prepare the objects to be written out:
  xAOD::MissingETComponentMap::iterator itr = result->begin();
  xAOD::MissingETComponentMap::iterator end = result->end();
  for( ; itr != end; ++itr ) {
    toPersistent( *itr );
  }

  ATH_MSG_DEBUG( "Transient map's ownership policy: " << trans->ownPolicy() );
  ATH_MSG_DEBUG( "Does the transient map track indices? " << (trans->trackIndices() ? "YES" : "NO") );
  ATH_MSG_DEBUG( "Does the transient map have a store? " << (trans->hasStore() ? "YES" : "NO") );

  ATH_MSG_DEBUG( "Persistent map's ownership policy: " << result->ownPolicy() );
  ATH_MSG_DEBUG( "Does the persistent map track indices? " << (result->trackIndices() ? "YES" : "NO") );
  ATH_MSG_DEBUG( "Does the persistent map have a store? " << (result->hasStore() ? "YES" : "NO") );

  // Return the new container:
  return result;
}

xAOD::MissingETComponentMap* xAODMissingETComponentMapCnv::createTransient() {

  // The known ID(s) for this container:
  static pool::Guid v1_guid( "994D9D32-820F-47B1-A54B-37C15CD0FD1E" );

  // Check if we're reading the most up to date type:
  if( compareClassGuid( v1_guid ) ) {
    ATH_MSG_DEBUG( "Before poolReadObject" );
    xAOD::MissingETComponentMap* c = poolReadObject< xAOD::MissingETComponentMap >();
    ATH_MSG_DEBUG( "New transient map's ownership policy: " << c->ownPolicy() );
    ATH_MSG_DEBUG( "Does the new transient map track indices? " << (c->trackIndices() ? "YES" : "NO") );
    ATH_MSG_DEBUG( "Does the new transient map have a store? " << (c->hasStore() ? "YES" : "NO") );
    ATH_MSG_DEBUG( "The new transient map's pointer: " << c );

    ATH_MSG_DEBUG( "SetStoreLink" );
    setStoreLink( c, m_key );
    ATH_MSG_DEBUG( "OK?" );
    return c;
  }

  // If we didn't recognise the ID, let's complain:
  throw std::runtime_error( "Unsupported version of "
			    "xAOD::MissingETComponentMap found" );
  return 0;
}

void xAODMissingETComponentMapCnv::toPersistent( xAOD::MissingETComponent* comp ) const {

  // Tell the object to prepare all its smart pointers for persistification:
  comp->updateLinks();

  return;
}
