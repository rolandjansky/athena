/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODPFOContainerCnv.h"

namespace {

  //copied from xAODMuonContainerCnv.cxx
  /** Setup link to objects aux store */
  void setStoreLink( SG::AuxVectorBase* cont, const std::string& key ) {
    DataLink< SG::IConstAuxStore > link( key + "Aux." );
    cont->setStore( link );
    return;
  }

}

xAODPFOContainerCnv::xAODPFOContainerCnv(ISvcLocator* svcLoc) : xAODPFOContainerCnvBase(svcLoc) {}

xAOD::PFOContainer* xAODPFOContainerCnv::createPersistent(xAOD::PFOContainer* trans){

  xAOD::PFOContainer* viewContainer = new xAOD::PFOContainer(trans->begin(),trans->end(), SG::VIEW_ELEMENTS);

  return viewContainer;

}

StatusCode xAODPFOContainerCnv::createObj( IOpaqueAddress* pAddr,
					   DataObject*& pObj ) {

  // Get the key of the container that we'll be creating:
  m_keyName = *( pAddr->par() + 1 );
  ATH_MSG_VERBOSE( "Key of xAOD::xAODPFOContainerCnv: " << m_keyName );

  // Let the base class do its thing now:
  return AthenaPoolConverter::createObj( pAddr, pObj );
} 

xAOD::PFOContainer* xAODPFOContainerCnv::createTransient(){

  static pool::Guid v1_guid( "476378BF-054D-499E-9CC9-000F501B30F2" );

  if( compareClassGuid( v1_guid ) ) {
    xAOD::PFOContainer* trans_v1 = poolReadObject< xAOD::PFOContainer >();
    setStoreLink( trans_v1, m_keyName );
    return trans_v1;
  }
  else throw std::runtime_error("Unsupported persistent version of xAOD::PFOContainer");

  return NULL;

}

void xAODPFOContainerCnv::toPersistent(xAOD::PFO* thePFO) const{

  thePFO->toPersistent();

}
