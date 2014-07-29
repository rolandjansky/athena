/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigSpacePointCountsContainerCnv.cxx

// System include(s):
#include <exception>

// Local include(s):
#include "xAODTrigSpacePointCountsContainerCnv.h"

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


xAODTrigSpacePointCountsContainerCnv::xAODTrigSpacePointCountsContainerCnv( ISvcLocator* svcLoc )
  : xAODTrigSpacePointCountsContainerCnvBase( svcLoc ) {

}

xAOD::TrigSpacePointCountsContainer*
xAODTrigSpacePointCountsContainerCnv::
createPersistent( xAOD::TrigSpacePointCountsContainer* trans ) {

  // Create a view copy of the container:
  xAOD::TrigSpacePointCountsContainer* result =
      new xAOD::TrigSpacePointCountsContainer( trans->begin(), trans->end(),
					  SG::VIEW_ELEMENTS );

  // Prepare the objects to be written out:
  xAOD::TrigSpacePointCountsContainer::iterator itr = result->begin();
  xAOD::TrigSpacePointCountsContainer::iterator end = result->end();
  for( ; itr != end; ++itr ) {
      toPersistent( *itr );
  }

  // Return the new container:
  return result;
}

/**
* This function needs to be re-implemented in order to figure out the StoreGate
* key of the container that's being created. After that's done, it lets the
* base class do its normal task.
*/
StatusCode xAODTrigSpacePointCountsContainerCnv::createObj(IOpaqueAddress* pAddr,
						DataObject*& pObj ) {
    
    // Get the key of the container that we'll be creating:
    m_key = *( pAddr->par() + 1 );
    ATH_MSG_VERBOSE( "Key of xAOD::TrigSpacePointCountsContainer: " << m_key );
    
    // Let the base class do its thing now:
    return AthenaPoolConverter::createObj( pAddr, pObj );
}


xAOD::TrigSpacePointCountsContainer* xAODTrigSpacePointCountsContainerCnv::createTransient() {

  // The known ID(s) for this container:
  static pool::Guid v1_guid( "519EBE61-C135-46C7-AD7D-C661DC6FC4B5" );

  // Check if we're reading the most up to date type:
  if( compareClassGuid( v1_guid ) ) {
    xAOD::TrigSpacePointCountsContainer* c = poolReadObject< xAOD::TrigSpacePointCountsContainer >();
    setStoreLink( c, m_key );
    return c;
  }

  // If we didn't recognise the ID, let's complain:
  throw std::runtime_error( "Unsupported version of "
			    "xAOD::TrigSpacePointCountsContainer found" );
  return 0;
}

void xAODTrigSpacePointCountsContainerCnv::
toPersistent( xAOD::TrigSpacePointCounts* obj ) const {
  ATH_MSG_VERBOSE("xAOD::TrigSpacePointCounts:   pixelClusEndcapC::contents::size:: "<<obj ->contentsPixelClusEndcapC().size() );
  ATH_MSG_VERBOSE("xAOD::TrigSpacePointCounts:   pixelClusBarrel::contents::size:: "<<obj ->contentsPixelClusBarrel().size() );
  ATH_MSG_VERBOSE("xAOD::TrigSpacePointCounts:   pixelClusEndcapA::contents::size:: "<<obj ->contentsPixelClusEndcapC().size()  );
  ATH_MSG_VERBOSE("xAOD::TrigSpacePointCounts:   pixelClusTotBins:: "<<obj ->pixelClusTotBins() );
  ATH_MSG_VERBOSE("xAOD::TrigSpacePointCounts:   pixelClusTotMin:: "<<obj ->pixelClusTotMin() );
  ATH_MSG_VERBOSE("xAOD::TrigSpacePointCounts:   pixelClusTotMax:: "<<obj ->pixelClusTotMax() );
  ATH_MSG_VERBOSE("xAOD::TrigSpacePointCounts:   pixelClusSizeBins:: "<<obj ->pixelClusSizeBins() );
  ATH_MSG_VERBOSE("xAOD::TrigSpacePointCounts:   pixelClusSizeMin:: "<<obj ->pixelClusSizeMin() );
  ATH_MSG_VERBOSE("xAOD::TrigSpacePointCounts:   pixelClusSizeMax:: "<<obj ->pixelClusSizeMax() );
  ATH_MSG_VERBOSE("xAOD::TrigSpacePointCounts:   sctSpEndcapC:: "<< obj ->sctSpEndcapC() );
  ATH_MSG_VERBOSE("xAOD::TrigSpacePointCounts:   sctSpBarrel:: "<< obj ->sctSpBarrel() );
  ATH_MSG_VERBOSE("xAOD::TrigSpacePointCounts:   sctSpEndcapA:: "<< obj ->sctSpEndcapA() );
  return;
}
