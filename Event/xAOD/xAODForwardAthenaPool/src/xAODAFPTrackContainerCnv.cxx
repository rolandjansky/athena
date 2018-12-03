/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Gaudi/Athena include(s):
#include "GaudiKernel/IOpaqueAddress.h"

// EDM include(s):
#include "xAODForward/versions/AFPTrackContainer_v1.h"

// Local include(s):
#include "xAODAFPTrackContainerCnv.h"
#include "xAODAFPTrackContainerCnv_v1.h"


namespace {

  /// Helper function setting up the container's link to its auxiliary store
  void setStoreLink( SG::AuxVectorBase* cont, const std::string& key ) {
    
    // The link to set up:
    DataLink< SG::IConstAuxStore > link( key + "Aux." );
   
    // Give it to the container:
    cont->setStore( link );
  }
    
} // private namespace
    
 
xAODAFPTrackContainerCnv::xAODAFPTrackContainerCnv( ISvcLocator* svcLoc )
  : xAODAFPTrackContainerCnvBase( svcLoc ) {
 
}
 
/**
 * * This function needs to be re-implemented in order to figure out the StoreGate
 * * key of the container that's being created. After that's done, it lets the
 * * base class do its normal task.
 * */
StatusCode xAODAFPTrackContainerCnv::createObj( IOpaqueAddress* pAddr,
						   DataObject*& pObj ) {
 
  // Get the key of the container that we'll be creating:
  m_key = *( pAddr->par() + 1 );
  ATH_MSG_VERBOSE( "Key of xAOD::AFPTrackContainer: " << m_key );
    
  // Let the base class do its thing now:
  return AthenaPoolConverter::createObj( pAddr, pObj ); 
}
 
xAOD::AFPTrackContainer* xAODAFPTrackContainerCnv::createPersistent( xAOD::AFPTrackContainer* trans )
{
  // Create a view copy of the container:
  xAOD::AFPTrackContainer* result =
    new xAOD::AFPTrackContainer( trans->begin(), trans->end(),
				    SG::VIEW_ELEMENTS );
 
  // Prepare the objects to be written out:
  xAOD::AFPTrackContainer::iterator itr = result->begin();
  xAOD::AFPTrackContainer::iterator end = result->end();
  for( ; itr != end; ++itr ) {
    toPersistent( *itr );
  }
 
  // Return the new container:
  return result;
}
 
xAOD::AFPTrackContainer* xAODAFPTrackContainerCnv::createTransient()
{
  // The known ID(s) for this container:
  static pool::Guid v1_guid( "EF887FA6-D17C-4D30-A2E6-3F46CA2EB44B" );
  static pool::Guid v2_guid( "2BCE47B1-7C55-48D0-AFA7-01563154AD02" );
 
  // Check if we're reading the most up to date type:
  if( compareClassGuid( v2_guid ) ) {
    xAOD::AFPTrackContainer* c =
      poolReadObject< xAOD::AFPTrackContainer >();
    setStoreLink( c, m_key );
    return c;
  } else if( compareClassGuid( v1_guid ) ) {

      // The v1 converter:
      static xAODAFPTrackContainerCnv_v1 converter;

      // Read in the v1 version:
      std::unique_ptr< xAOD::AFPTrackContainer_v1 >
         old( poolReadObject< xAOD::AFPTrackContainer_v1 >() );

      // Return the converted object:
      xAOD::AFPTrackContainer* c =
         converter.createTransient( old.get(), msg() );
      setStoreLink( c, m_key );
      return c;
   }
 
 
  // If we didn't recognise the ID, let's complain:
  throw std::runtime_error( "Unsupported version of "
			    "xAOD::AFPTrackContainer found" );
  return nullptr;
}
 
void xAODAFPTrackContainerCnv::toPersistent( xAOD::AFPTrack* track ) const {
 
  // Tell the object to prepare all its smart pointers for persistification:
  track->toPersistent();
}
