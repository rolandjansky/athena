/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "xAODAFPToFTrackContainerCnv.h"

// Gaudi/Athena include(s):
#include "GaudiKernel/IOpaqueAddress.h"

// EDM include(s):
#include "xAODForward/versions/AFPToFTrackContainer_v1.h"

// System include(s):
#include <exception>

namespace {

  /// Helper function setting up the container's link to its auxiliary store
  void setStoreLink( SG::AuxVectorBase* cont, const std::string& key ) {
    
    // The link to set up:
    DataLink< SG::IConstAuxStore > link( key + "Aux." );
   
    // Give it to the container:
    cont->setStore( link );
  }
    
} // private namespace
    
 
xAODAFPToFTrackContainerCnv::xAODAFPToFTrackContainerCnv( ISvcLocator* svcLoc )
  : xAODAFPToFTrackContainerCnvBase( svcLoc ) {
 
}
 
/**
 * * This function needs to be re-implemented in order to figure out the StoreGate
 * * key of the container that's being created. After that's done, it lets the
 * * base class do its normal task.
 * */
StatusCode xAODAFPToFTrackContainerCnv::createObj( IOpaqueAddress* pAddr,
						   DataObject*& pObj ) {
 
  // Get the key of the container that we'll be creating:
  m_key = *( pAddr->par() + 1 );
  ATH_MSG_VERBOSE( "Key of xAOD::AFPToFTrackContainer: " << m_key );
    
  // Let the base class do its thing now:
  return AthenaPoolConverter::createObj( pAddr, pObj ); 
}
 
xAOD::AFPToFTrackContainer* xAODAFPToFTrackContainerCnv::createPersistent( xAOD::AFPToFTrackContainer* trans )
{
  // Create a view copy of the container:
  xAOD::AFPToFTrackContainer* result =
    new xAOD::AFPToFTrackContainer( trans->begin(), trans->end(),
				    SG::VIEW_ELEMENTS );
 
  // Prepare the objects to be written out:
  xAOD::AFPToFTrackContainer::iterator itr = result->begin();
  xAOD::AFPToFTrackContainer::iterator end = result->end();
  for( ; itr != end; ++itr ) {
    toPersistent( *itr );
  }
 
  // Return the new container:
  return result;
}
 
xAOD::AFPToFTrackContainer* xAODAFPToFTrackContainerCnv::createTransient()
{
  // The known ID(s) for this container:
  static pool::Guid v1_guid( "C2373AB6-252C-44AE-9523-252CA8E03292" );
 
  // Check if we're reading the most up to date type:
  if( compareClassGuid( v1_guid ) ) {

      // Return the converted object:
      xAOD::AFPToFTrackContainer* c =
         poolReadObject< xAOD::AFPToFTrackContainer >();
      setStoreLink( c, m_key );
      return c;
   }
 


 
  // If we didn't recognise the ID, let's complain:
  throw std::runtime_error( "Unsupported version of "
			    "xAOD::AFPToFTrackContainer found" );
  return nullptr;
}
 
void xAODAFPToFTrackContainerCnv::toPersistent( xAOD::AFPToFTrack* track ) const {
 
  // Tell the object to prepare all its smart pointers for persistification:
  track->toPersistent();
}
