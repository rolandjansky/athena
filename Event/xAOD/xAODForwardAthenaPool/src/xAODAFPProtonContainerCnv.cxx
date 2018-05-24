/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   xAODAFPProtonContainerCnv.cxx
 * @author Krzysztof Cieśla <krzysztof.marcin.ciesla@cern.ch>
 * @date   2018-04-05
 * 
 * @brief  Implementation of class xAODAFPProtonContainerCnv. Based on xAODAFPSiHitsClusterContainerCnv.cxx
 * 
 */


// System include(s):
#include <exception>
#include <iostream>

// Gaudi/Athena include(s):
#include "GaudiKernel/IOpaqueAddress.h"

// Local include(s):
#include "xAODAFPProtonContainerCnv.h"


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
    
 
xAODAFPProtonContainerCnv::xAODAFPProtonContainerCnv( ISvcLocator* svcLoc )
  : xAODAFPProtonContainerCnvBase( svcLoc ) {
 
}
 
/**
 * This function needs to be re-implemented in order to figure out the StoreGate
 * key of the container that's being created. After that's done, it lets the
 * base class do its normal task.
 */
StatusCode xAODAFPProtonContainerCnv::createObj( IOpaqueAddress* pAddr,
							DataObject*& pObj ) {
 
  // Get the key of the container that we'll be creating:
  m_key = *( pAddr->par() + 1 );
  ATH_MSG_VERBOSE( "Key of xAOD::AFPProtonContainer: " << m_key );
    
  // Let the base class do its thing now:
  return AthenaPoolConverter::createObj( pAddr, pObj ); 
}
 
xAOD::AFPProtonContainer* xAODAFPProtonContainerCnv::createPersistent( xAOD::AFPProtonContainer* trans )
{
 
  // Create a view copy of the container:
  xAOD::AFPProtonContainer* result =
    new xAOD::AFPProtonContainer( trans->begin(), trans->end(),
					 SG::VIEW_ELEMENTS );
 
  // Prepare the objects to be written out:
  xAOD::AFPProtonContainer::iterator itr = result->begin();
  xAOD::AFPProtonContainer::iterator end = result->end();
  for( ; itr != end; ++itr ) {
    toPersistent( *itr );
  }
 
  // Return the new container:
  return result;
}
 
xAOD::AFPProtonContainer* xAODAFPProtonContainerCnv::createTransient()
{
  // The known ID(s) for this container:
  static pool::Guid v1_guid( "D6DBFC87-D5C9-40CD-BFD0-21B19D0ED7AB" );
 
  // Check if we're reading the most up to date type:
  if( compareClassGuid( v1_guid ) ) {
    xAOD::AFPProtonContainer* c =
      poolReadObject< xAOD::AFPProtonContainer >();
    setStoreLink( c, m_key );
    return c;
  }
 
  // If we didn't recognise the ID, let's complain:
  throw std::runtime_error( "Unsupported version of "
			    "xAOD::AFPProtonContainer found" );
  return 0;
}
 
void xAODAFPProtonContainerCnv::toPersistent( xAOD::AFPProton* proton ) const {
 
  // Tell the object to prepare all its smart pointers for persistification:
  proton->toPersistent();
  
  return;
}

