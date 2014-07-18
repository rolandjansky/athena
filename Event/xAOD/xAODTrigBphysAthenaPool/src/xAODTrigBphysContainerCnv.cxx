/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigBphysContainerCnv.cxx

// System include(s):
#include <exception>

// Local include(s):
#include "xAODTrigBphysContainerCnv.h"

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


xAODTrigBphysContainerCnv::xAODTrigBphysContainerCnv( ISvcLocator* svcLoc )
   : xAODTrigBphysContainerCnvBase( svcLoc ) {

}

xAOD::TrigBphysContainer*
xAODTrigBphysContainerCnv::
createPersistent( xAOD::TrigBphysContainer* trans ) {

   // Create a view copy of the container:
   xAOD::TrigBphysContainer* result =
      new xAOD::TrigBphysContainer( trans->begin(), trans->end(),
					  SG::VIEW_ELEMENTS );

   // Prepare the objects to be written out:
   xAOD::TrigBphysContainer::iterator itr = result->begin();
   xAOD::TrigBphysContainer::iterator end = result->end();
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
StatusCode xAODTrigBphysContainerCnv::createObj(IOpaqueAddress* pAddr,
                                                DataObject*& pObj ) {
    
    // Get the key of the container that we'll be creating:
    m_key = *( pAddr->par() + 1 );
    ATH_MSG_VERBOSE( "Key of xAOD::TrigBphysContainer: " << m_key );
    
    // Let the base class do its thing now:
    return AthenaPoolConverter::createObj( pAddr, pObj );
}


xAOD::TrigBphysContainer* xAODTrigBphysContainerCnv::createTransient() {

   // The known ID(s) for this container:
    //      static pool::Guid v1_guid( "0C7F7869-C67A-4E4A-8793-358F8B32DFEA" ); // JW - the original version
    static pool::Guid v1_guid( "FD05137E-421B-40B5-AB7C-D119C5490784" ); // JW a test version
    

   // Check if we're reading the most up to date type:
   if( compareClassGuid( v1_guid ) ) {
       xAOD::TrigBphysContainer* c = poolReadObject< xAOD::TrigBphysContainer >();
       setStoreLink( c, m_key );
       return c;
   }

   // If we didn't recognise the ID, let's complain:
    
    throw std::runtime_error( "Unsupported version of "
                             "xAOD::TrigBphysContainer found" );
   return 0;
}

void xAODTrigBphysContainerCnv::
toPersistent( xAOD::TrigBphys* obj ) const {

    // secondaryDecay
    //    typedef const ElementLink< xAOD::TrigBphys >              trigBphysLink_t;
    //    typedef const ElementLink< xAOD::TrackParticleContainer > elTPcontainer_t;

    //    trigBphysLink_t::reference el = const_cast<trigBphysLink_t::reference>( obj->secondaryDecayLink());
    // const ElementLink< xAOD::TrigBphys > & el = const_cast<ElementLink< xAOD::TrigBphys > & >( obj->secondaryDecayLink());
    // el.toPersistent();
    // is this needed?
    
    //associated tracks
//    std::vector< ElementLink< xAOD::TrackParticleContainer > >::const_iterator cit  = obj->trackParticleLinks().begin();
//    std::vector< ElementLink< xAOD::TrackParticleContainer > >::const_iterator cend = obj->trackParticleLinks().end();
    
//    for (; cit != cend;++cit) {
        //elTPcontainer_t::reference cel = const_cast<elTPcontainer_t::reference >(*cit);
        //cel.toPersistent();
        //const ElementLink< xAOD::TrackParticleContainer >::reference cel = const_cast< ElementLink< xAOD::TrackParticleContainer > >(*cit);
        //cel.toPersistent();
//    } // for
    
   return;
}
