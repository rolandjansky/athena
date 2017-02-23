/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 
// $Id: xAODMissingETAssociationMapCnv.cxx 795699 2017-02-05 23:26:05Z khoo $

// System include(s):
#include <exception>

// Local include(s):
#include "xAODMissingETAssociationMapCnv.h"

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

xAODMissingETAssociationMapCnv::xAODMissingETAssociationMapCnv( ISvcLocator* svcLoc )
  : xAODMissingETAssociationMapCnvBase( svcLoc ) {

}

/**
 * This function needs to be re-implemented in order to figure out the StoreGate
 * key of the container that's being created. After that's done, it lets the
 * base class do its normal task.
 */
StatusCode xAODMissingETAssociationMapCnv::createObj( IOpaqueAddress* pAddr,
						    DataObject*& pObj ) {

  // Get the key of the container that we'll be creating:
  m_key = *( pAddr->par() + 1 );
  ATH_MSG_VERBOSE( "Key of xAOD::MissingETAssociationMap: " << m_key );

  // Let the base class do its thing now:
  return AthenaPoolConverter::createObj( pAddr, pObj );
}

xAOD::MissingETAssociationMap*
xAODMissingETAssociationMapCnv::createPersistent( xAOD::MissingETAssociationMap* trans ) {

  // Create a view copy of the container:
  xAOD::MissingETAssociationMap* result =
    new xAOD::MissingETAssociationMap( trans->begin(), trans->end(), SG::VIEW_ELEMENTS );

  // // Prepare the objects to be written out:
  // xAOD::MissingETAssociationMap::iterator itr = result->begin();
  // xAOD::MissingETAssociationMap::iterator end = result->end();
  // for( ; itr != end; ++itr ) {
  //   toPersistent( *itr );
  // }

  // Return the new container:
  return result;
}

xAOD::MissingETAssociationMap* xAODMissingETAssociationMapCnv::createTransient() {

  // The known ID(s) for this container:
  static pool::Guid v1_guid( "E2EAA116-F03E-430E-B4EF-216AAF7DEEE8" );

  // Check if we're reading the most up to date type:
  if( compareClassGuid( v1_guid ) ) {
    xAOD::MissingETAssociationMap* c = poolReadObject< xAOD::MissingETAssociationMap >();

    setStoreLink( c, m_key );
    return c;
  }

  // If we didn't recognise the ID, let's complain:
  throw std::runtime_error( "Unsupported version of "
			    "xAOD::MissingETAssociationMap found" );
  return 0;
}

// void xAODMissingETAssociationMapCnv::toPersistent( xAOD::MissingETAssociation* comp ) const {

//   // Tell the object to prepare all its smart pointers for persistification:
//   comp->updateLinks();

//   return;
// }
