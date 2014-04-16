/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 
// $Id: xAODMissingETContainerCnv.cxx 586838 2014-03-08 15:10:34Z khoo $

// System include(s):
#include <exception>

// Local include(s):
#include "xAODMissingETContainerCnv.h"

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

xAODMissingETContainerCnv::xAODMissingETContainerCnv( ISvcLocator* svcLoc )
  : xAODMissingETContainerCnvBase( svcLoc ) {

}

/**
 * This function needs to be re-implemented in order to figure out the StoreGate
 * key of the container that's being created. After that's done, it lets the
 * base class do its normal task.
 */
StatusCode xAODMissingETContainerCnv::createObj( IOpaqueAddress* pAddr,
						 DataObject*& pObj ) {

  // Get the key of the container that we'll be creating:
  m_key = *( pAddr->par() + 1 );
  ATH_MSG_VERBOSE( "Key of xAOD::MissingETContainer: " << m_key );

  // Let the base class do its thing now:
  return AthenaPoolConverter::createObj( pAddr, pObj );
}

xAOD::MissingETContainer*
xAODMissingETContainerCnv::createPersistent( xAOD::MissingETContainer* trans ) {

  // Create a view copy of the container:
  xAOD::MissingETContainer* result =
    new xAOD::MissingETContainer( trans->begin(), trans->end(), SG::VIEW_ELEMENTS );

  // xAOD::MissingET contains only simple data, no special treatment needed.

  // // Prepare the objects to be written out:
  // xAOD::MissingETContainer::iterator itr = result->begin();
  // xAOD::MissingETContainer::iterator end = result->end();
  // for( ; itr != end; ++itr ) {
  //   toPersistent( *itr );
  // }

  // Return the new container:
  return result;
}

xAOD::MissingETContainer* xAODMissingETContainerCnv::createTransient() {

  // The known ID(s) for this container:
  static pool::Guid v1_guid( "F49162FE-6BC0-49BC-A7DA-A792136BD939" );

  // Check if we're reading the most up to date type:
  if( compareClassGuid( v1_guid ) ) {
    xAOD::MissingETContainer* c = poolReadObject< xAOD::MissingETContainer >();
    setStoreLink( c, m_key );
    return c;
  }

  // If we didn't recognise the ID, let's complain:
  throw std::runtime_error( "Unsupported version of "
			    "xAOD::MissingETContainer found" );
  return 0;
}

// void xAODMissingETContainerCnv::toPersistent( xAOD::MissingET* /*met*/ ) const {

//   // Tell the object to prepare all its smart pointers for persistification:
//   met->toPersistent();

//   return;
// }
