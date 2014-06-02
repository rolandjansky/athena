/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODCompositeParticleContainerCnv.h"

namespace {

  /** Setup link to objects aux store */
  void setStoreLink( SG::AuxVectorBase* cont, const std::string& key ) {
    DataLink< SG::IConstAuxStore > link( key + "Aux." );
    cont->setStore( link );
    return;
  }

}


xAODCompositeParticleContainerCnv::xAODCompositeParticleContainerCnv(ISvcLocator* svcLoc) : xAODCompositeParticleContainerCnvBase(svcLoc) {}


xAOD::CompositeParticleContainer* xAODCompositeParticleContainerCnv::createPersistent(xAOD::CompositeParticleContainer* trans){

  xAOD::CompositeParticleContainer* viewContainer = new xAOD::CompositeParticleContainer(trans->begin(),trans->end(), SG::VIEW_ELEMENTS);

  xAOD::CompositeParticleContainer::iterator firstCompositeParticle = viewContainer->begin();
  xAOD::CompositeParticleContainer::iterator lastCompositeParticle = viewContainer->end();

  for (; firstCompositeParticle != lastCompositeParticle; ++firstCompositeParticle) {
    toPersistent(*firstCompositeParticle);
  }

  return viewContainer;
}


StatusCode xAODCompositeParticleContainerCnv::createObj( IOpaqueAddress* pAddr,
                                                         DataObject*& pObj ) {

  // Get the key of the container that we'll be creating:
  m_keyName = *( pAddr->par() + 1 );
  ATH_MSG_VERBOSE( "Key of xAOD::xAODCompositeParticleContainerCnv: " << m_keyName );

  // Let the base class do its thing now:
  return AthenaPoolConverter::createObj( pAddr, pObj );
}


xAOD::CompositeParticleContainer* xAODCompositeParticleContainerCnv::createTransient() {

  static pool::Guid v1_guid( "C65FFC20-CC41-4C6D-BEDF-B10E935EBFCC" );

  if ( compareClassGuid( v1_guid ) ) {
    xAOD::CompositeParticleContainer* trans_v1 = poolReadObject< xAOD::CompositeParticleContainer >();
    setStoreLink( trans_v1, m_keyName );
    return trans_v1;
  }
  else throw std::runtime_error("Unsupported persistent version of xAOD::CompositeParticleContainer");

  return NULL;
}


void xAODCompositeParticleContainerCnv::toPersistent(xAOD::CompositeParticle* theCompositeParticle) const{

  theCompositeParticle->toPersistent();
}
