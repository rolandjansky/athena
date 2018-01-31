/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Gaudi/Athena include(s):
#include "GaudiKernel/IOpaqueAddress.h"

// EDM include(s):
#include "xAODForward/versions/AFPSiHitContainer_v1.h"

// Local include(s):
#include "xAODAFPSiHitContainerCnv.h"
#include "xAODAFPSiHitContainerCnv_v1.h"


namespace {

  /// Helper function setting up the container's link to its auxiliary store
  void setStoreLink( SG::AuxVectorBase* cont, const std::string& key ) {
    
    // The link to set up:
    DataLink< SG::IConstAuxStore > link( key + "Aux." );
   
    // Give it to the container:
    cont->setStore( link );
  }
    
} // private namespace
    
 
xAODAFPSiHitContainerCnv::xAODAFPSiHitContainerCnv( ISvcLocator* svcLoc )
  : xAODAFPSiHitContainerCnvBase( svcLoc ) {
 
}
 
/**
 * * This function needs to be re-implemented in order to figure out the StoreGate
 * * key of the container that's being created. After that's done, it lets the
 * * base class do its normal task.
 * */
StatusCode xAODAFPSiHitContainerCnv::createObj( IOpaqueAddress* pAddr,
						   DataObject*& pObj ) {
 
  // Get the key of the container that we'll be creating:
  m_key = *( pAddr->par() + 1 );
  ATH_MSG_VERBOSE( "Key of xAOD::AFPSiHitContainer: " << m_key );
    
  // Let the base class do its thing now:
  return AthenaPoolConverter::createObj( pAddr, pObj ); 
}

xAOD::AFPSiHitContainer* xAODAFPSiHitContainerCnv::createPersistent( xAOD::AFPSiHitContainer* trans )
{
  // Create a view copy of the container:
  xAOD::AFPSiHitContainer* result =
    new xAOD::AFPSiHitContainer( trans->begin(), trans->end(),
				    SG::VIEW_ELEMENTS );
 
  // Prepare the objects to be written out:
  xAOD::AFPSiHitContainer::iterator itr = result->begin();
  xAOD::AFPSiHitContainer::iterator end = result->end();
  for( ; itr != end; ++itr ) {
    toPersistent( *itr );
  }
 
  // Return the new container:
  return result;
}

xAOD::AFPSiHitContainer* xAODAFPSiHitContainerCnv::createTransient()
{
  // The known ID(s) for this container:
  static pool::Guid v1_guid( "6BE5A00E-A13D-4904-9358-CB28D4C57879" );
  static pool::Guid v2_guid( "11AC58B6-10C2-4F0B-8E1F-B99E823F274E" );
 
  // Check if we're reading the most up to date type:
  if( compareClassGuid( v2_guid ) ) {
    xAOD::AFPSiHitContainer* c =
      poolReadObject< xAOD::AFPSiHitContainer >();
    setStoreLink( c, m_key );
    return c;
  } else if( compareClassGuid( v1_guid ) ) {

      // The v1 converter:
      static xAODAFPSiHitContainerCnv_v1 converter;

      // Read in the v1 version:
      std::unique_ptr< xAOD::AFPSiHitContainer_v1 >
         old( poolReadObject< xAOD::AFPSiHitContainer_v1 >() );

      // Return the converted object:
      xAOD::AFPSiHitContainer* c =
         converter.createTransient( old.get(), msg() );
      setStoreLink( c, m_key );
      return c;
   }
 
  // If we didn't recognise the ID, let's complain:
  throw std::runtime_error( "Unsupported version of "
			    "xAOD::AFPSiHitContainer found" );
  return nullptr;
}
 
void xAODAFPSiHitContainerCnv::toPersistent( xAOD::AFPSiHit* hit ) const {
 
  // Tell the object to prepare all its smart pointers for persistification:
  hit->toPersistent();
}

