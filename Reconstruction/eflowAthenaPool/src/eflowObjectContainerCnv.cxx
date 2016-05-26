/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Framework includes
#include "eflowObjectContainerCnv.h"
#include "eflowEventTPCnv/eflowObjectCnv_p1.h"
#include "eflowEventTPCnv/eflowObjectContainerCnv_p1.h"
#include "eflowEventTPCnv/eflowObjectCnv_p2.h"
#include "eflowEventTPCnv/eflowObjectContainerCnv_p2.h"
#include "eflowEventTPCnv/eflowObjectCnv_p3.h"
#include "eflowEventTPCnv/eflowObjectContainerCnv_p3.h"
#include "eflowEventTPCnv/eflowObjectCnv_p4.h"
#include "eflowEventTPCnv/eflowObjectContainerCnv_p4.h"
#include "eflowEventTPCnv/eflowObjectCnv_p5.h"
#include "eflowEventTPCnv/eflowObjectContainerCnv_p5.h"
#include "GaudiKernel/MsgStream.h"

eflowObjectContainer_p5* 
eflowObjectContainerCnv::createPersistent( eflowObjectContainer* transCont ) 
{
  MsgStream eflowStream( msgSvc(), "eflowObjectContainerCnv" );

  eflowObjectContainerCnv_p5 cnv;
  eflowObjectContainer_p5 *persObj = cnv.createPersistent( transCont, eflowStream );

  if (eflowStream.level() <= MSG::DEBUG) eflowStream << MSG::DEBUG << "::createPersistent [Success]" << endreq;
  return persObj; 
}

eflowObjectContainer* eflowObjectContainerCnv::createTransient() 
{
  MsgStream eflowStream( msgSvc(), "eflowObjectContainerCnv" );

  eflowObjectContainer *transObj = 0;

  static pool::Guid tr_guid("0044A49A-14B8-45BE-9E62-B9780F226168");
  static pool::Guid p1_guid("1DE97911-24C0-429D-9041-258A2F1CA7DF");
  static pool::Guid p2_guid("E9674EC4-80D0-4616-AE98-26488ADE726B");
  static pool::Guid p3_guid("2DF2EC16-AA4D-4BF9-82F8-7DFCE3BE00A0");
  static pool::Guid p4_guid("75CC8E85-4CA8-4FE9-A001-26856E3D44C6");
  static pool::Guid p5_guid("A38F4620-AB4A-11E2-90EB-00241D7D6914");

  if ( compareClassGuid(tr_guid) ) {

    // regular object from before the T/P separation
    return poolReadObject<eflowObjectContainer>();

  } else if ( compareClassGuid(p1_guid) ) {

    // using auto_ptr ensures deletion of the persistent object
    std::auto_ptr<eflowObjectContainer_p1> persObj( poolReadObject<eflowObjectContainer_p1>() );
    eflowObjectContainerCnv_p1 cnv;
    transObj = cnv.createTransient( persObj.get(), eflowStream );
  } else if (compareClassGuid(p2_guid) ) {
    
    // using auto_ptr ensures deletion of the persistent object
    std::auto_ptr<eflowObjectContainer_p2> persObj( poolReadObject<eflowObjectContainer_p2>() );
    eflowObjectContainerCnv_p2 cnv;
    transObj = cnv.createTransient( persObj.get(), eflowStream );

  } else if (compareClassGuid(p3_guid) ) {

    // using auto_ptr ensures deletion of the persistent object                                                                                                                                                  
    std::auto_ptr<eflowObjectContainer_p3> persObj( poolReadObject<eflowObjectContainer_p3>() );
    eflowObjectContainerCnv_p3 cnv;
    transObj = cnv.createTransient( persObj.get(), eflowStream );
  } else if (compareClassGuid(p4_guid) ) {

    // using auto_ptr ensures deletion of the persistent object                                                                                                                                                  
    std::auto_ptr<eflowObjectContainer_p4> persObj( poolReadObject<eflowObjectContainer_p4>() );
    eflowObjectContainerCnv_p4 cnv;
    transObj = cnv.createTransient( persObj.get(), eflowStream );
  }  else if (compareClassGuid(p5_guid) ) {

    // using auto_ptr ensures deletion of the persistent object                                                                                                                                                  
    std::auto_ptr<eflowObjectContainer_p5> persObj( poolReadObject<eflowObjectContainer_p5>() );
    eflowObjectContainerCnv_p5 cnv;
    transObj = cnv.createTransient( persObj.get(), eflowStream );
  }
  else throw std::runtime_error("Unsupported persistent version of eflowObjectContainer");

  return transObj;
}
