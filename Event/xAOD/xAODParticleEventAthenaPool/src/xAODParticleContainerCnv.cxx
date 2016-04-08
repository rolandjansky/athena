/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODParticleContainerCnv.h"

namespace {

  /** Setup link to objects aux store */
  void setStoreLink( SG::AuxVectorBase* cont, const std::string& key ) {
    DataLink< SG::IConstAuxStore > link( key + "Aux." );
    cont->setStore( link );
    return;
  }

}


xAODParticleContainerCnv::xAODParticleContainerCnv(ISvcLocator* svcLoc)
  : xAODParticleContainerCnvBase(svcLoc)
{ }


xAOD::ParticleContainer*
xAODParticleContainerCnv::createPersistent(xAOD::ParticleContainer* trans){

  xAOD::ParticleContainer* viewContainer =
    new xAOD::ParticleContainer(trans->begin(),trans->end(), SG::VIEW_ELEMENTS);

  xAOD::ParticleContainer::iterator firstParticle = viewContainer->begin();
  xAOD::ParticleContainer::iterator lastParticle  = viewContainer->end();

  for (; firstParticle != lastParticle; ++firstParticle) {
    toPersistent(*firstParticle);
  }

  return viewContainer;
}


StatusCode xAODParticleContainerCnv::createObj( IOpaqueAddress* pAddr,
                                                DataObject*& pObj ) {

  // Get the key of the container that we'll be creating:
  m_keyName = *( pAddr->par() + 1 );
  ATH_MSG_VERBOSE( "Key of xAOD::xAODParticleContainerCnv: " << m_keyName );

  // Let the base class do its thing now:
  return AthenaPoolConverter::createObj( pAddr, pObj );
}


xAOD::ParticleContainer* xAODParticleContainerCnv::createTransient() {

  static pool::Guid v1_guid( "CE9D717A-A6DD-4BCA-A946-63A730E0EA3B" );

  if ( compareClassGuid( v1_guid ) ) {
    xAOD::ParticleContainer* trans_v1 = poolReadObject< xAOD::ParticleContainer >();
    setStoreLink( trans_v1, m_keyName );
    return trans_v1;
  }
  else throw std::runtime_error("Unsupported persistent version of xAOD::ParticleContainer");

  return NULL;
}


void xAODParticleContainerCnv::toPersistent(xAOD::Particle* theParticle) const{

  theParticle->toPersistent();
}
