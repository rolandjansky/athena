/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PlanarClusterContainerCnv.h"
#include "GaudiKernel/MsgStream.h"

#include <memory>

#include <iostream>

PlanarClusterContainerCnv::PlanarClusterContainerCnv (ISvcLocator* svcloc) :
  PlanarClusterContainerCnvBase(svcloc)
 {}

PlanarClusterContainerCnv::~PlanarClusterContainerCnv() {}


iFatras::PlanarClusterContainer* PlanarClusterContainerCnv::createTransient() {
  
  static pool::Guid   p1_guid("E0411C25-64DD-49D9-AC1A-8E32A75C8ABF"); // with PlanarCluster_p1
  static pool::Guid   p2_guid("8697937F-1E3B-4C80-876E-0D6467C50688"); // with PlanarCluster_p2
  ATH_MSG_DEBUG("createTransient(): main converter");
  
  iFatras::PlanarClusterContainer* p_collection(0);
  if( compareClassGuid(p2_guid) ) {
    ATH_MSG_DEBUG("createTransient(): T/P version 2 detected");
    std::auto_ptr< iFatras::PlanarClusterContainer_p2 >  p_coll( poolReadObject< iFatras::PlanarClusterContainer_p2 >() );
    p_collection = m_converter_p2.createTransient( p_coll.get(), msg() );
  } else if ( compareClassGuid(p1_guid) ) {
    ATH_MSG_DEBUG("createTransient(): T/P version 1 detected");
    std::auto_ptr< iFatras::PlanarClusterContainer_p1 >  p_coll( poolReadObject< iFatras::PlanarClusterContainer_p1 >() );
    p_collection = m_converter_p1.createTransient( p_coll.get(), msg() );
  } else {
    throw std::runtime_error("Unsupported persistent version of PlanarClusterContainer");
  }
  return p_collection;
}

PlanarClusterContainer_PERS* PlanarClusterContainerCnv::createPersistent (iFatras::PlanarClusterContainer* transCont)
{
  PlanarClusterContainer_PERS *pldc_p= m_converter_p2.createPersistent( transCont, msg() );
  
  return pldc_p;
}
