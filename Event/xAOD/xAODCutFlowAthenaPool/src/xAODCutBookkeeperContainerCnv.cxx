/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODCutBookkeeperContainerCnv.h"

namespace {

  /** Setup link to objects aux store */
  void setStoreLink( SG::AuxVectorBase* cont, const std::string& key ) {
    DataLink< SG::IConstAuxStore > link( key + "Aux." );
    cont->setStore( link );
    return;
  }

}


xAODCutBookkeeperContainerCnv::xAODCutBookkeeperContainerCnv(ISvcLocator* svcLoc) : xAODCutBookkeeperContainerCnvBase(svcLoc) {}


xAOD::CutBookkeeperContainer* xAODCutBookkeeperContainerCnv::createPersistent(xAOD::CutBookkeeperContainer* trans){

  xAOD::CutBookkeeperContainer* viewContainer = new xAOD::CutBookkeeperContainer(trans->begin(),trans->end(), SG::VIEW_ELEMENTS);

  xAOD::CutBookkeeperContainer::iterator firstCutBookkeeper = viewContainer->begin();
  xAOD::CutBookkeeperContainer::iterator lastCutBookkeeper  = viewContainer->end();

  for (; firstCutBookkeeper != lastCutBookkeeper; ++firstCutBookkeeper) {
    toPersistent(*firstCutBookkeeper);
  }

  return viewContainer;
}


StatusCode xAODCutBookkeeperContainerCnv::createObj( IOpaqueAddress* pAddr,
                                                       DataObject*& pObj ) {

  // Get the key of the container that we'll be creating:
  m_keyName = *( pAddr->par() + 1 );
  ATH_MSG_VERBOSE( "Key of xAOD::xAODCutBookkeeperContainerCnv: " << m_keyName );

  // Let the base class do its thing now:
  return AthenaPoolConverter::createObj( pAddr, pObj );
}


xAOD::CutBookkeeperContainer* xAODCutBookkeeperContainerCnv::createTransient() {

  static pool::Guid v1_guid( "F2F90B2F-B879-43B8-AF9B-0F843E299A87" );

  if ( compareClassGuid( v1_guid ) ) {
    xAOD::CutBookkeeperContainer* trans_v1 = poolReadObject< xAOD::CutBookkeeperContainer >();
    setStoreLink( trans_v1, m_keyName );
    return trans_v1;
  }
  else throw std::runtime_error("Unsupported persistent version of xAOD::CutBookkeeperContainer");

  return NULL;
}


void xAODCutBookkeeperContainerCnv::toPersistent(xAOD::CutBookkeeper* theCutBookkeeper) const{

  theCutBookkeeper->toPersistent();
}
